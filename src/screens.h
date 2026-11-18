#ifndef CODEQUEST_SCREENS_H
#define CODEQUEST_SCREENS_H

#include "learning.h"
#include "duel.h"
#include "progress.h"

typedef enum {
    SCREEN_MAP, SCREEN_LESSON, SCREEN_PROGRESS, SCREEN_PRACTICE, SCREEN_RESULTS, SCREEN_TRACE, SCREEN_EXAM,
    SCREEN_CHAPTER_DONE, SCREEN_DUEL
} Screen;

enum { ARRAYS_TRACE, POINTERS_TRACE, PARAMETERS_TRACE };
enum {
    FOUNDATIONS_TOPIC, LOOPS_TOPIC, ARRAYS_TOPIC, GRID_TOPIC,
    STRINGS_TOPIC, PARAMETERS_TOPIC, POINTERS_TOPIC, LISTS_TOPIC, TOPIC_COUNT
};

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
    int questionIndex;              /* position of currentQuestion in its lesson */
    /* Chapter run: only the first answer to each lesson question counts toward the score. */
    bool generatedQuestion;         /* extra Arrays question, never scored */
    bool firstAnswerGiven;
    int runCorrect;
    int runScore;                   /* percentage shown on the chapter summary */
    int previousBest;
    int wrongAttempts;
    bool showingSolution;
    Progress progress;
    Walkthrough traces[TRACE_COUNT];
    int traceStep;
    ExamState exam;
    DuelState duel;
    int questionSubmissions;
} AppState;

void DrawApp(AppState *app);

#endif
