#ifndef CODEQUEST_SCREENS_H
#define CODEQUEST_SCREENS_H

#include "learning.h"

typedef enum {
    SCREEN_MAP, SCREEN_LESSON, SCREEN_PROGRESS, SCREEN_PRACTICE, SCREEN_RESULTS, SCREEN_TRACE
} Screen;

enum { ARRAYS_TRACE, POINTERS_TRACE, PARAMETERS_TRACE };
enum {
    FOUNDATIONS_TOPIC, LOOPS_TOPIC, ARRAYS_TOPIC, GRID_TOPIC,
    STRINGS_TOPIC, PARAMETERS_TOPIC, POINTERS_TOPIC, LISTS_TOPIC, TOPIC_COUNT
};

typedef struct {
    Screen screen;
    int selectedTopic;
    int selectedAnswer;
    bool lessonAvailable[TOPIC_COUNT];
    Lesson lessons[TOPIC_COUNT];
    Question currentQuestion;
    int wrongAttempts;
    bool showingSolution;
    bool practicePassed[TOPIC_COUNT];
    Walkthrough traces[TRACE_COUNT];
    int traceStep;
} AppState;

void DrawApp(AppState *app);

#endif
