#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdint.h>
#include <wchar.h>
#include <limits.h>
#include "progress.h"

static HANDLE lock = INVALID_HANDLE_VALUE;
static wchar_t savePath[MAX_PATH], backupPath[MAX_PATH], tempPath[MAX_PATH];
static bool writable, validPrimary;

typedef enum { MISSING, INVALID, UNSUPPORTED, VALID } LoadResult;

enum { SAVE_VERSION = 2 };
static bool loadedVersion1;     /* set when the last valid read was a version 1 save */

BadgeTier BadgeForScore(int score)
{
    if (score >= MASTER_SCORE) return BADGE_MASTER;
    if (score >= EXPERT_SCORE) return BADGE_EXPERT;
    if (score >= AVERAGE_SCORE) return BADGE_AVERAGE;
    return BADGE_NONE;
}

const char *BadgeName(BadgeTier tier)
{
    switch (tier) {
        case BADGE_AVERAGE: return "Average";
        case BADGE_EXPERT: return "Expert";
        case BADGE_MASTER: return "Master";
        default: return "No badge";
    }
}

/* Import the earlier exam build's save once. It recorded practice, but no lesson or trace visits. */
static bool ImportLegacy(TopicProgress *topics)
{
    wchar_t path[MAX_PATH];
    DWORD length = GetModuleFileNameW(NULL, path, MAX_PATH);
    if (!length || length >= MAX_PATH) return false;
    wchar_t *name = wcsrchr(path, L'\\');
    if (!name || name - path + 14 >= MAX_PATH) return false;
    wcscpy(name + 1, L"progress.dat");
    FILE *file = _wfopen(path, L"rb");
    if (!file) return false;
    enum { BODY_WORDS = 2 + PROGRESS_TOPICS * 3 + 1 };
    uint32_t words[BODY_WORDS + 1];
    bool complete = fread(words, sizeof words[0], BODY_WORDS + 1, file) == BODY_WORDS + 1 &&
        fgetc(file) == EOF && !ferror(file);
    fclose(file);
    if (!complete || words[0] != 0x43515356u || words[1] != 1) return false;
    uint32_t hash = 2166136261u;
    for (int i = 0; i < BODY_WORDS; i++)
        for (int shift = 0; shift < 32; shift += 8)
            hash = (hash ^ ((words[i] >> shift) & 0xffu)) * 16777619u;
    if (hash != words[BODY_WORDS]) return false;
    for (int i = 0; i < PROGRESS_TOPICS; i++) {
        const uint32_t *entry = &words[2 + i * 3];
        if (entry[1] > entry[0] || entry[2] > 7) return false;
    }
    for (int i = 0; i < PROGRESS_TOPICS; i++) {
        const uint32_t *entry = &words[2 + i * 3];
        // Only attempts carry over: chapter badges need a full chapter score.
        topics[i].attempts = entry[0] > INT_MAX ? INT_MAX : (int)entry[0];
        topics[i].bestScore = NO_SCORE;
    }
    return true;
}

static LoadResult ReadSave(const wchar_t *path, TopicProgress *topics)
{
    FILE *file = _wfopen(path, L"rb");
    if (!file) {
        DWORD attributes = GetFileAttributesW(path);
        return attributes == INVALID_FILE_ATTRIBUTES && GetLastError() == ERROR_FILE_NOT_FOUND
            ? MISSING : INVALID;
    }
    char buffer[4096];
    size_t size = fread(buffer, 1, sizeof buffer - 1, file);
    bool complete = feof(file) && !ferror(file);
    fclose(file);
    if (!complete) return INVALID;
    buffer[size] = '\0';
    const char *end = NULL;
    cJSON *root = cJSON_ParseWithLengthOpts(buffer, size + 1, &end, true);
    if (!root || end != buffer + size) {
        cJSON_Delete(root);
        return INVALID;
    }
    const cJSON *version = cJSON_GetObjectItemCaseSensitive(root, "version");
    const cJSON *entries = cJSON_GetObjectItemCaseSensitive(root, "topics");
    bool known = cJSON_IsNumber(version) &&
        (version->valuedouble == 1 || version->valuedouble == SAVE_VERSION);
    bool legacy = known && version->valuedouble == 1;
    LoadResult result = cJSON_IsNumber(version) && !known ? UNSUPPORTED : INVALID;
    if (known && cJSON_IsArray(entries) && cJSON_GetArraySize(entries) == PROGRESS_TOPICS) {
        TopicProgress loaded[PROGRESS_TOPICS] = {0};
        result = VALID;
        for (int i = 0; i < PROGRESS_TOPICS; i++) {
            const cJSON *entry = cJSON_GetArrayItem(entries, i);
            const cJSON *viewed = cJSON_GetObjectItemCaseSensitive(entry, "viewed");
            const cJSON *traced = cJSON_GetObjectItemCaseSensitive(entry, "traced");
            const cJSON *attempts = cJSON_GetObjectItemCaseSensitive(entry, "attempts");
            const cJSON *best = cJSON_GetObjectItemCaseSensitive(entry, "bestScore");
            // Version 1 saves recorded single-question practice, so they have no chapter score.
            bool bestValid = legacy || (cJSON_IsNumber(best) &&
                best->valuedouble == best->valueint &&
                best->valueint >= NO_SCORE && best->valueint <= 100);
            if (!cJSON_IsBool(viewed) || !cJSON_IsBool(traced) ||
                !cJSON_IsNumber(attempts) || attempts->valueint < 0 ||
                attempts->valuedouble != attempts->valueint || !bestValid) {
                result = INVALID;
                break;
            }
            loaded[i] = (TopicProgress){cJSON_IsTrue(viewed), cJSON_IsTrue(traced),
                attempts->valueint, legacy ? NO_SCORE : best->valueint};
        }
        if (result == VALID) {
            for (int i = 0; i < PROGRESS_TOPICS; i++) topics[i] = loaded[i];
            loadedVersion1 = legacy;
        }
    }
    cJSON_Delete(root);
    return result;
}

void OpenProgress(Progress *progress)
{
    wchar_t base[MAX_PATH], directory[MAX_PATH], lockPath[MAX_PATH];
    for (int i = 0; i < PROGRESS_TOPICS; i++) progress->topics[i] = (TopicProgress){.bestScore = NO_SCORE};
    DWORD length = GetEnvironmentVariableW(L"LOCALAPPDATA", base, MAX_PATH);
    if (!length || length >= MAX_PATH - 32) {
        progress->notice = "Local progress folder unavailable. Learning is still available.";
        return;
    }
    swprintf(directory, MAX_PATH, L"%ls\\CodeQuest", base);
    swprintf(savePath, MAX_PATH, L"%ls\\progress.json", directory);
    swprintf(backupPath, MAX_PATH, L"%ls\\progress.bak", directory);
    swprintf(tempPath, MAX_PATH, L"%ls\\progress.tmp", directory);
    swprintf(lockPath, MAX_PATH, L"%ls\\progress.lock", directory);
    CreateDirectoryW(directory, NULL);
    lock = CreateFileW(lockPath, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (lock == INVALID_HANDLE_VALUE) {
        ReadSave(savePath, progress->topics);
        progress->notice = "Progress is read-only. Close other CodeQuest windows and restart.";
        return;
    }
    LoadResult primary = ReadSave(savePath, progress->topics);
    validPrimary = primary == VALID;
    writable = validPrimary || primary == MISSING;
    if (validPrimary) {
        if (loadedVersion1)
            progress->notice = "Badges now come from chapter scores. Finish a chapter to earn one.";
        return;
    }
    if (primary == UNSUPPORTED) {
        progress->notice = "This save uses a different version. Progress is read-only.";
        return;
    }
    LoadResult backup = ReadSave(backupPath, progress->topics);
    if (backup == VALID) {
        if (primary == INVALID) {
            wchar_t damaged[MAX_PATH];
            swprintf(damaged, MAX_PATH, L"%ls\\progress.damaged", directory);
            writable = MoveFileExW(savePath, damaged, MOVEFILE_WRITE_THROUGH);
        }
        progress->notice = writable ? "Progress restored from the backup." :
            "Backup loaded. The damaged save is preserved and progress is read-only.";
    } else if (primary != MISSING || backup != MISSING) {
        writable = false;
        progress->notice = "Saved progress could not be read. The files have been preserved.";
    } else if (ImportLegacy(progress->topics)) {
        SaveProgress(progress);
        if (!progress->retry) progress->notice = "Earlier practice progress imported.";
    }
}

void SaveProgress(Progress *progress)
{
    if (!writable) return;
    // Write the new snapshot first. Keep the existing save until validation succeeds.
    FILE *file = _wfopen(tempPath, L"wb");
    bool ok = file != NULL;
    if (file) {
        fprintf(file, "{\"version\":%d,\"topics\":[", SAVE_VERSION);
        for (int i = 0; i < PROGRESS_TOPICS; i++) {
            TopicProgress t = progress->topics[i];
            fprintf(file, "%s{\"viewed\":%s,\"traced\":%s,\"attempts\":%d,\"bestScore\":%d}",
                i ? "," : "", t.viewed ? "true" : "false", t.traced ? "true" : "false",
                t.attempts, t.bestScore);
        }
        fprintf(file, "]}");
        ok = !ferror(file);
        if (fclose(file) != 0) ok = false;
    }
    TopicProgress checked[PROGRESS_TOPICS];
    ok = ok && ReadSave(tempPath, checked) == VALID;
    if (ok && validPrimary) {
        wchar_t pendingBackup[MAX_PATH];
        swprintf(pendingBackup, MAX_PATH, L"%ls.tmp", backupPath);
        ok = CopyFileW(savePath, pendingBackup, FALSE) &&
            MoveFileExW(pendingBackup, backupPath, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
    }
    if (ok) ok = MoveFileExW(tempPath, savePath, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
    if (ok) validPrimary = true;
    progress->retry = !ok;
    progress->notice = ok ? NULL : "Progress could not be saved. Retry from My progress.";
}

void CloseProgress(void)
{
    if (lock != INVALID_HANDLE_VALUE) CloseHandle(lock);
    lock = INVALID_HANDLE_VALUE;
    writable = false;
    validPrimary = false;
}
