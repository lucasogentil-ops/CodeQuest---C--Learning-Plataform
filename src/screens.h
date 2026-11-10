#ifndef CODEQUEST_SCREENS_H
#define CODEQUEST_SCREENS_H

#include "learning.h"
#include "progress.h"

typedef enum {
    SCREEN_MAP, SCREEN_LESSON, SCREEN_PROGRESS, SCREEN_PRACTICE, SCREEN_RESULTS, SCREEN_TRACE,
    SCREEN_EXAM
} Screen;

enum { ARRAYS_TRACE, POINTERS_TRACE, PARAMETERS_TRACE };
enum {
    FOUNDATIONS_TOPIC, LOOPS_TOPIC, ARRAYS_TOPIC, GRID_TOPIC,
    STRINGS_TOPIC, PARAMETERS_TOPIC, POINTERS_TOPIC, LISTS_TOPIC, TOPIC_COUNT
};
_Static_assert((int)TOPIC_COUNT == (int)PROGRESS_TOPICS, "progress.h must track every topic");

enum { EXAM_LENGTH = 5 };
typedef enum { EXAM_INTRO, EXAM_QUESTION, EXAM_SUMMARY } ExamPhase;

typedef struct {
    ExamPhase phase;
    int count;                      /* questions in this exam (up to EXAM_LENGTH) */
    int index;                      /* current question */
    int selected;
    int score;
    int topics[EXAM_LENGTH];
    Question questions[EXAM_LENGTH];
    bool correct[EXAM_LENGTH];
} ExamState;

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
    Progress progress;
    ProgressStatus loadStatus;
    int questionSubmissions;   /* answers submitted to the current question */
    uint32_t newBadges;        /* badges earned by the latest submission */
    bool saveFailed;
    ExamState exam;
} AppState;

void DrawApp(AppState *app);

#endif