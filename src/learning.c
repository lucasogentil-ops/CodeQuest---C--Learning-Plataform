#include "learning.h"
#include <cjson/cJSON.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>

static bool CopyText(const cJSON *item, char *output, size_t capacity)
{
    const char *value = cJSON_GetStringValue(item);
    if (!value || !value[0] || strlen(value) >= capacity) return false;
    strcpy(output, value);
    return true;
}

static bool ReadText(const cJSON *object, const char *key, char *output, size_t capacity)
{
    return CopyText(cJSON_GetObjectItemCaseSensitive(object, key), output, capacity);
}

static bool ReadWalkthrough(const cJSON *root, Walkthrough *trace)
{
    const cJSON *lines = cJSON_GetObjectItemCaseSensitive(root, "lines");
    const cJSON *steps = cJSON_GetObjectItemCaseSensitive(root, "steps");
    trace->lineCount = cJSON_GetArraySize(lines);
    trace->stepCount = cJSON_GetArraySize(steps);
    if (!ReadText(root, "title", trace->title, sizeof trace->title) ||
        !ReadText(root, "language", trace->language, sizeof trace->language) ||
        !cJSON_IsArray(lines) || trace->lineCount < 1 || trace->lineCount > TRACE_LIMIT ||
        !cJSON_IsArray(steps) || trace->stepCount < 1 || trace->stepCount > TRACE_LIMIT) return false;

    for (int i = 0; i < trace->lineCount; i++)
        if (!CopyText(cJSON_GetArrayItem(lines, i), trace->lines[i], sizeof trace->lines[i]))
            return false;

    for (int i = 0; i < trace->stepCount; i++) {
        const cJSON *entry = cJSON_GetArrayItem(steps, i);
        const cJSON *line = cJSON_GetObjectItemCaseSensitive(entry, "line");
        const cJSON *values = cJSON_GetObjectItemCaseSensitive(entry, "values");
        TraceStep *step = &trace->steps[i];
        if (!cJSON_IsNumber(line) || line->valuedouble != line->valueint ||
            line->valueint < 0 || line->valueint >= trace->lineCount ||
            !ReadText(entry, "explanation", step->explanation, sizeof step->explanation) ||
            !cJSON_IsArray(values) || cJSON_GetArraySize(values) != TRACE_VALUES) return false;
        step->line = line->valueint;
        for (int j = 0; j < TRACE_VALUES; j++)
            if (!CopyText(cJSON_GetArrayItem(values, j), step->values[j], sizeof step->values[j]))
                return false;
    }
    return true;
}

bool LoadWalkthrough(const char *path, Walkthrough *trace)
{
    char *file = LoadFileText(path);
    if (!file) return false;
    cJSON *root = cJSON_ParseWithOpts(file, NULL, true);
    UnloadFileText(file);
    Walkthrough loaded = {0};
    bool valid = ReadWalkthrough(root, &loaded);
    cJSON_Delete(root);
    if (valid) *trace = loaded;
    return valid;
}

static bool ReadQuestion(const cJSON *object, Question *question)
{
    const cJSON *options = cJSON_GetObjectItemCaseSensitive(object, "options");
    if (!cJSON_IsArray(options) || cJSON_GetArraySize(options) != OPTION_COUNT ||
        !ReadText(object, "prompt", question->prompt, sizeof question->prompt) ||
        !ReadText(object, "code", question->code, sizeof question->code) ||
        !ReadText(object, "hint", question->hint, sizeof question->hint) ||
        !ReadText(object, "correctOptionId", question->correctOptionId,
                  sizeof question->correctOptionId)) return false;

    const char *gap = strstr(question->code, "/* Missing line */");
    if (!gap || strstr(gap + sizeof "/* Missing line */" - 1, "/* Missing line */"))
        return false;

    bool foundCorrectAnswer = false;
    for (int i = 0; i < OPTION_COUNT; i++) {
        const cJSON *entry = cJSON_GetArrayItem(options, i);
        AnswerOption *option = &question->options[i];
        if (!ReadText(entry, "id", option->id, sizeof option->id) ||
            !ReadText(entry, "code", option->code, sizeof option->code) ||
            !ReadText(entry, "feedback", option->feedback, sizeof option->feedback)) return false;
        for (int previous = 0; previous < i; previous++) {
            if (strcmp(option->id, question->options[previous].id) == 0 ||
                strcmp(option->code, question->options[previous].code) == 0) return false;
        }
        if (IsCorrectAnswer(question, i)) foundCorrectAnswer = true;
    }
    return foundCorrectAnswer;
}

bool LoadLesson(const char *path, Lesson *lesson)
{
    char *file = LoadFileText(path);
    if (!file) return false;
    cJSON *root = cJSON_ParseWithOpts(file, NULL, true);
    UnloadFileText(file);

    Lesson loaded = {0};
    bool valid = cJSON_IsObject(root) &&
        ReadText(root, "title", loaded.title, sizeof loaded.title) &&
        ReadText(root, "explanation", loaded.explanation, sizeof loaded.explanation) &&
        ReadText(root, "code", loaded.code, sizeof loaded.code) &&
        ReadQuestion(cJSON_GetObjectItemCaseSensitive(root, "question"), &loaded.question);
    cJSON_Delete(root);
    if (valid) *lesson = loaded;
    return valid;
}

bool IsCorrectAnswer(const Question *question, int selection)
{
    return selection >= 0 && selection < OPTION_COUNT &&
        strcmp(question->options[selection].id, question->correctOptionId) == 0;
}

void GenerateArraysQuestion(Question *question)
{
    const char *positions[] = {"first", "second", "third"};
    int values[] = {GetRandomValue(1, 9), GetRandomValue(1, 9), GetRandomValue(1, 9)};
    int target = GetRandomValue(0, 2);
    int other = (target + GetRandomValue(1, 2)) % 3;
    int amount = GetRandomValue(1, 9);
    bool subtract = GetRandomValue(0, 1) == 1;
    char sign = subtract ? '-' : '+';
    char oppositeSign = subtract ? '+' : '-';
    int result = subtract ? values[target] - amount : values[target] + amount;
    *question = (Question){.options = {{.id = "update"}, {.id = "index"}, {.id = "sign"}},
                           .correctOptionId = "update"};

    snprintf(question->prompt, sizeof question->prompt,
             subtract ? "Which line subtracts %d from the %s value?" :
                        "Which line adds %d to the %s value?",
             amount, positions[target]);
    snprintf(question->code, sizeof question->code,
             "int values[3] = {%d, %d, %d};\n/* Missing line */",
             values[0], values[1], values[2]);
    snprintf(question->hint, sizeof question->hint,
             "Count from index zero. Match the operator to the requested change.");

    AnswerOption *update = &question->options[0];
    AnswerOption *index = &question->options[1];
    AnswerOption *wrongSign = &question->options[2];
    snprintf(update->code, sizeof update->code, "values[%d] %c= %d;", target, sign, amount);
    snprintf(update->feedback, sizeof update->feedback,
             "Index %d selects the %s element.\n%d %c %d gives %d.",
             target, positions[target], values[target], sign, amount, result);
    snprintf(index->code, sizeof index->code, "values[%d] %c= %d;", other, sign, amount);
    snprintf(index->feedback, sizeof index->feedback,
             "Index %d selects the %s element.\nUse index %d for the %s element.",
             other, positions[other], target, positions[target]);
    snprintf(wrongSign->code, sizeof wrongSign->code, "values[%d] %c= %d;",
             target, oppositeSign, amount);
    snprintf(wrongSign->feedback, sizeof wrongSign->feedback,
             "%c= %s the amount.\nThis question asks you to %s it, so use %c=.",
             oppositeSign, subtract ? "adds" : "subtracts", subtract ? "subtract" : "add", sign);

    // Swap whole options so each ID stays with its code and feedback.
    for (int i = OPTION_COUNT - 1; i > 0; i--) {
        int j = GetRandomValue(0, i);
        AnswerOption temporary = question->options[i];
        question->options[i] = question->options[j];
        question->options[j] = temporary;
    }
}
