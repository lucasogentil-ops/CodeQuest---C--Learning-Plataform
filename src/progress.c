#include "progress.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

enum { SAVE_MAGIC = 0x43515356u /* "CQSV" */, SAVE_VERSION = 1,
       HEADER_WORDS = 2, TOPIC_WORDS = 3,
       BODY_WORDS = HEADER_WORDS + PROGRESS_TOPICS * TOPIC_WORDS + 1,
       FILE_WORDS = BODY_WORDS + 1 };

static uint32_t Checksum(const uint32_t *words, int count)
{
    uint32_t hash = 2166136261u; /* FNV-1a over the bytes of each word */
    for (int i = 0; i < count; i++)
        for (int shift = 0; shift < 32; shift += 8) {
            hash ^= (words[i] >> shift) & 0xFFu;
            hash *= 16777619u;
        }
    return hash;
}

void ProgressReset(Progress *progress) { memset(progress, 0, sizeof *progress); }

ProgressStatus ProgressLoad(const char *path, Progress *progress)
{
    ProgressReset(progress);
    FILE *file = fopen(path, "rb");
    if (!file) return PROGRESS_MISSING;

    uint32_t words[FILE_WORDS];
    size_t read = fread(words, sizeof words[0], FILE_WORDS, file);
    bool extra = fgetc(file) != EOF;
    fclose(file);
    if (read != FILE_WORDS || extra || words[0] != SAVE_MAGIC ||
        words[1] != SAVE_VERSION || Checksum(words, BODY_WORDS) != words[BODY_WORDS])
        return PROGRESS_CORRUPT;

    Progress loaded;
    ProgressReset(&loaded);
    for (int t = 0; t < PROGRESS_TOPICS; t++) {
        const uint32_t *w = &words[HEADER_WORDS + t * TOPIC_WORDS];
        if (w[1] > w[0] || (w[2] >> BADGE_KINDS) != 0) return PROGRESS_CORRUPT;
        loaded.topics[t] = (TopicProgress){w[0], w[1], w[2]};
    }
    loaded.globalBadges = words[BODY_WORDS - 1];
    if (loaded.globalBadges > BADGE_MASTER) return PROGRESS_CORRUPT;
    *progress = loaded;
    return PROGRESS_LOADED;
}

bool ProgressSave(const char *path, const Progress *progress)
{
    uint32_t words[FILE_WORDS] = {SAVE_MAGIC, SAVE_VERSION};
    for (int t = 0; t < PROGRESS_TOPICS; t++) {
        uint32_t *w = &words[HEADER_WORDS + t * TOPIC_WORDS];
        w[0] = progress->topics[t].attempts;
        w[1] = progress->topics[t].correct;
        w[2] = progress->topics[t].badges;
    }
    words[BODY_WORDS - 1] = progress->globalBadges;
    words[BODY_WORDS] = Checksum(words, BODY_WORDS);

    char temporary[512];
    if (snprintf(temporary, sizeof temporary, "%s.tmp", path) >= (int)sizeof temporary)
        return false;
    FILE *file = fopen(temporary, "wb");
    if (!file) return false;
    bool written = fwrite(words, sizeof words[0], FILE_WORDS, file) == FILE_WORDS;
    written = fflush(file) == 0 && written;
    written = fclose(file) == 0 && written;
    if (!written) { remove(temporary); return false; }
#ifdef _WIN32
    bool replaced = MoveFileExA(temporary, path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    bool replaced = rename(temporary, path) == 0;
#endif
    if (!replaced) remove(temporary);
    return replaced;
}

int ProgressCompletedTopics(const Progress *progress)
{
    int count = 0;
    for (int t = 0; t < PROGRESS_TOPICS; t++)
        if (progress->topics[t].badges & BADGE_COMPLETED) count++;
    return count;
}

uint32_t ProgressRecordAnswer(Progress *progress, int topic, bool correct, bool firstSubmission)
{
    if (topic < 0 || topic >= PROGRESS_TOPICS) return 0;
    TopicProgress *entry = &progress->topics[topic];
    uint32_t before = entry->badges, globalBefore = progress->globalBadges;

    if (entry->attempts < UINT32_MAX) entry->attempts++;
    if (correct) {
        if (entry->correct < UINT32_MAX) entry->correct++;
        entry->badges |= BADGE_COMPLETED;
        entry->badges |= firstSubmission ? BADGE_FIRST_TRY : BADGE_PERSISTENT;
    }
    if (ProgressCompletedTopics(progress) == PROGRESS_TOPICS)
        progress->globalBadges |= BADGE_MASTER;

    return (entry->badges & ~before) | (((progress->globalBadges & ~globalBefore)) << 8);
}