#ifndef CODEQUEST_LEARNING_H
#define CODEQUEST_LEARNING_H

#include <stdbool.h>

enum { OPTION_COUNT = 3, NO_SELECTION = -1 };

typedef struct {
    char id[16];
    char code[80];
    char feedback[240];
} AnswerOption;

typedef struct {
    char prompt[120];
    char code[240];
    char hint[240];
    AnswerOption options[OPTION_COUNT];
    char correctOptionId[16];
} Question;

typedef struct {
    char title[100];
    char explanation[480];
    char code[240];
    Question question;
} Lesson;

enum { TRACE_COUNT = 3, TRACE_LIMIT = 8, TRACE_VALUES = 3 };

typedef struct {
    int line;
    char explanation[240];
    char values[TRACE_VALUES][80];
} TraceStep;

typedef struct {
    char title[100];
    char language[16];
    int lineCount;
    char lines[TRACE_LIMIT][80];
    int stepCount;
    TraceStep steps[TRACE_LIMIT];
} Walkthrough;

bool LoadLesson(const char *path, Lesson *lesson);
bool LoadWalkthrough(const char *path, Walkthrough *trace);
bool IsCorrectAnswer(const Question *question, int selection);
void GenerateArraysQuestion(Question *question);

#endif
