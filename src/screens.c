#include "screens.h"
#include "ui.h"
#include <stddef.h>
#include <limits.h>

_Static_assert((int)TOPIC_COUNT == (int)PROGRESS_TOPICS, "Save topic count must match map");

static const Walkthrough *TopicTrace(const AppState *app)
{
    switch (app->selectedTopic) {
        case ARRAYS_TOPIC: return &app->traces[ARRAYS_TRACE];
        case POINTERS_TOPIC: return &app->traces[POINTERS_TRACE];
        case PARAMETERS_TOPIC: return &app->traces[PARAMETERS_TRACE];
        default: return NULL;
    }
}

static bool OpenWalkthrough(AppState *app, Rectangle bounds)
{
    const Walkthrough *trace = TopicTrace(app);
    if (!trace || trace->stepCount == 0) return false;
    if (!Button(bounds, "Walkthrough", true)) return false;
    app->traceStep = 0;
    app->screen = SCREEN_TRACE;
    return true;
}

static const char *topicTitles[] = {
    "C foundations", "Decisions and loops", "Arrays", "2D arrays",
    "Strings and scope", "Functions and parameters", "Pointers", "Linked lists"
};

static void DrawMap(AppState *app)
{
    Label("YOUR LEARNING PATH", 40, 122, 18, CQ_PURPLE);
    Label("Small steps. Clearer code.", 40, 158, 36, CQ_TEXT);
    Label("Choose any topic to explore. Every topic stays available.", 40, 210, 20, CQ_MUTED);

    for (int i = 0; i < TOPIC_COUNT; i++) {
        int x = 40 + (i%2)*392;
        int y = 270 + (i/2)*92;
        if (TopicButton((Rectangle){(float)x, (float)y, 372, 80}, topicTitles[i], i + 1)) {
            app->selectedTopic = i;
            app->screen = SCREEN_LESSON;
        }
    }
    DrawPanel((Rectangle){850, 270, 390, 356});
    Label("YOUR FIRST STEP", 878, 296, 16, CQ_PALE);
    Label("Start with Arrays", 878, 332, 25, CQ_TEXT);
    Label("Short lessons. Visual examples.", 878, 380, 20, CQ_MUTED);
    Label("Practice at your own pace.", 878, 410, 20, CQ_MUTED);
    if (Button((Rectangle){878, 466, 334, 50}, "Explore Arrays", true)) {
        app->selectedTopic = ARRAYS_TOPIC;
        app->screen = SCREEN_LESSON;
    }
    Label("Your badges", 878, 545, 21, CQ_TEXT);
    Label("Your first badge is ahead of you.", 878, 581, 18, CQ_MUTED);
}

static void DrawLesson(AppState *app)
{
    int topic = app->selectedTopic;
    if (app->lessonAvailable[topic]) {
        if (!app->progress.topics[topic].viewed) {
            app->progress.topics[topic].viewed = true;
            SaveProgress(&app->progress);
        }
        const Lesson *lesson = &app->lessons[topic];
        Label(TextFormat("%s / Lesson", topicTitles[topic]), 40, 122, 18, CQ_PURPLE);
        Label(lesson->title, 40, 158, 36, CQ_TEXT);
        DrawPanel((Rectangle){40, 250, 650, 376});
        Label("Key idea", 68, 282, 26, CQ_TEXT);
        Label(lesson->explanation, 68, 340, 24, CQ_MUTED);
        DrawPanel((Rectangle){714, 250, 526, 376});
        Label("C example", 742, 282, 26, CQ_TEXT);
        Label(lesson->code, 742, 345, 24, CQ_TEXT);
        if (OpenWalkthrough(app, (Rectangle){742, 544, 202, 52})) return;
        if (Button((Rectangle){960, 544, 252, 52}, "Try a question", true)) {
            app->currentQuestion = lesson->question;
            app->selectedAnswer = NO_SELECTION;
            app->wrongAttempts = 0;
            app->questionSubmissions = 0;
            app->showingSolution = false;
            app->screen = SCREEN_PRACTICE;
        }
        return;
    }
    Label("LESSON UNAVAILABLE", 40, 122, 18, CQ_PURPLE);
    Label(topicTitles[topic], 40, 158, 36, CQ_TEXT);
    DrawPanel((Rectangle){40, 270, 720, 350});
    Label("Could not load the lesson.", 68, 300, 28, CQ_TEXT);
    Label("Check the lesson file, rebuild and restart.", 68, 356, 24, CQ_MUTED);
    DrawPanel((Rectangle){790, 270, 450, 350});
    Label("Learning journey", 818, 300, 26, CQ_TEXT);
    Label("Read the lesson", 818, 364, 22, CQ_TEXT);
    Label("Step through an example", 818, 410, 22, CQ_TEXT);
    Label("Try a missing-code question", 818, 456, 22, CQ_TEXT);
    if (OpenWalkthrough(app, (Rectangle){818, 502, 394, 48})) return;
    if (Button((Rectangle){818, 560, 394, 48}, "Back to topic map", false)) app->screen = SCREEN_MAP;
}

static void DrawWalkthrough(AppState *app)
{
    const Walkthrough *trace = TopicTrace(app);
    const TraceStep *step = &trace->steps[app->traceStep];
    TopicProgress *progress = &app->progress.topics[app->selectedTopic];
    if (app->traceStep == trace->stepCount - 1 && !progress->traced) {
        progress->traced = true;
        SaveProgress(&app->progress);
    }
    Label(TextFormat("%s / WALKTHROUGH", trace->language), 40, 122, 18, CQ_PURPLE);
    Label(trace->title, 40, 158, 36, CQ_TEXT);
    Label(TextFormat("Step %d of %d  |  Values shown at this step",
                     app->traceStep + 1, trace->stepCount), 40, 210, 20, CQ_MUTED);
    DrawPanel((Rectangle){40, 250, 624, 306});
    DrawPanel((Rectangle){688, 250, 552, 306});
    Label("Example code", 68, 270, 22, CQ_TEXT);
    for (int i = 0; i < trace->lineCount; i++) {
        int y = 307 + i * 28;
        bool active = i == step->line;
        if (active) DrawRectangleRounded((Rectangle){60, (float)(y - 3), 584, 28}, 0.2f, 8, CQ_CARD);
        Label(TextFormat("%s %d", active ? ">" : " ", i + 1), 72, y, 17,
              active ? CQ_PALE : CQ_MUTED);
        Label(trace->lines[i], 122, y, 19, CQ_TEXT);
    }
    Label("Values and targets", 716, 270, 22, CQ_TEXT);
    for (int i = 0; i < TRACE_VALUES; i++) {
        int y = 308 + i * 43;
        DrawRectangleRounded((Rectangle){716, (float)y, 496, 36}, 0.25f, 8, CQ_CARD);
        Label(step->values[i], 732, y + 5, 19, CQ_TEXT);
    }
    Label(step->explanation, 716, 457, 20, CQ_TEXT);

    int nextStep = app->traceStep;
    if (app->traceStep > 0) {
        if (Button((Rectangle){40, 581, 146, 50}, "Previous", false)) nextStep--;
    } else {
        Label("First step", 75, 594, 18, CQ_MUTED);
    }
    if (app->traceStep + 1 < trace->stepCount) {
        if (Button((Rectangle){204, 581, 146, 50}, "Next", true)) nextStep++;
    } else {
        Label("Last step", 240, 594, 18, CQ_MUTED);
    }
    if (Button((Rectangle){368, 581, 146, 50}, "Reset", false)) nextStep = 0;
    app->traceStep = nextStep;
    if (Button((Rectangle){1000, 581, 240, 50}, "Back to topic", false)) app->screen = SCREEN_LESSON;
}

static void StartNewQuestion(AppState *app)
{
    GenerateArraysQuestion(&app->currentQuestion);
    app->selectedAnswer = NO_SELECTION;
    app->wrongAttempts = 0;
    app->questionSubmissions = 0;
    app->showingSolution = false;
    app->screen = SCREEN_PRACTICE;
}

static void DrawPractice(AppState *app)
{
    const Question *question = &app->currentQuestion;
    Label(TextFormat("%s / Practice", topicTitles[app->selectedTopic]), 40, 122, 18, CQ_PURPLE);
    Label(question->prompt, 40, 158, 36, CQ_TEXT);
    DrawPanel((Rectangle){40, 250, 570, 376});
    Label("C example", 68, 282, 26, CQ_TEXT);
    Label(question->code, 68, 345, 24, CQ_TEXT);
    DrawPanel((Rectangle){634, 250, 606, 376});
    Label("Choose one answer", 662, 282, 26, CQ_TEXT);
    for (int i = 0; i < OPTION_COUNT; i++) {
        const char *label = TextFormat("%c. %s", 'A' + i, question->options[i].code);
        if (Button((Rectangle){662, (float)(330 + 60*i), 550, 48}, label, app->selectedAnswer == i))
            app->selectedAnswer = i;
    }
    if (app->selectedAnswer == NO_SELECTION) {
        Label("Select an answer to continue.", 662, 557, 20, CQ_MUTED);
    } else if (Button((Rectangle){662, 544, 220, 52}, "Check answer", true)) {
        TopicProgress *progress = &app->progress.topics[app->selectedTopic];
        if (progress->attempts < INT_MAX) progress->attempts++;
        if (IsCorrectAnswer(question, app->selectedAnswer)) {
            progress->solved = true;
            if (app->questionSubmissions == 0) progress->firstTry = true;
            else progress->persistent = true;
        } else if (app->wrongAttempts < 2)
            app->wrongAttempts++;
        if (app->questionSubmissions < INT_MAX) app->questionSubmissions++;
        app->screen = SCREEN_RESULTS;
        SaveProgress(&app->progress);
    }
    if (app->selectedTopic == ARRAYS_TOPIC &&
        Button((Rectangle){982, 544, 230, 52}, "New question", false))
        StartNewQuestion(app);
}

static void DrawResults(AppState *app)
{
    const Question *question = &app->currentQuestion;
    bool correct = IsCorrectAnswer(question, app->selectedAnswer);
    Label(TextFormat("%s / Result", topicTitles[app->selectedTopic]), 40, 122, 18, CQ_PURPLE);
    const char *heading = correct ? "Correct!" : "Give it another try";
    if (app->showingSolution) heading = "Solution";
    Label(heading, 40, 158, 36, CQ_TEXT);
    DrawPanel((Rectangle){40, 250, 1200, 376});
    const AnswerOption *answer = &question->options[app->selectedAnswer];
    if (app->showingSolution) {
        for (int i = 0; i < OPTION_COUNT; i++)
            if (IsCorrectAnswer(question, i)) answer = &question->options[i];
    }
    Label(app->showingSolution ? "Correct answer" : "Your answer", 68, 282, 24, CQ_MUTED);
    Label(answer->code, 68, 326, 26, CQ_TEXT);
    Label(correct || app->showingSolution ? answer->feedback : question->hint, 68, 396, 24, CQ_TEXT);
    if (Button((Rectangle){68, 544, 225, 52}, "Back to topic map", correct))
        app->screen = SCREEN_MAP;
    if (Button((Rectangle){310, 544, 205, 52}, "Try again", !correct)) {
        app->selectedAnswer = NO_SELECTION;
        if (correct || app->showingSolution) {
            app->wrongAttempts = 0;
            app->questionSubmissions = 0;
        }
        app->showingSolution = false;
        app->screen = SCREEN_PRACTICE;
    }
    if (!correct && app->wrongAttempts >= 2 && !app->showingSolution &&
        Button((Rectangle){532, 544, 220, 52}, "Show solution", false)) {
        app->showingSolution = true;
        app->progress.topics[app->selectedTopic].solution = true;
        SaveProgress(&app->progress);
    }
    if (app->selectedTopic == ARRAYS_TOPIC &&
        Button((Rectangle){770, 544, 220, 52}, "New question", false))
        StartNewQuestion(app);
}

static void ShuffleOptions(Question *question)
{
    // Swap whole options so each ID stays with its code and feedback.
    for (int i = OPTION_COUNT - 1; i > 0; i--) {
        int j = GetRandomValue(0, i);
        AnswerOption temporary = question->options[i];
        question->options[i] = question->options[j];
        question->options[j] = temporary;
    }
}

static int AvailableTopics(const AppState *app, int topics[TOPIC_COUNT])
{
    int count = 0;
    for (int i = 0; i < TOPIC_COUNT; i++)
        if (app->lessonAvailable[i]) topics[count++] = i;
    return count;
}

static void StartExam(AppState *app)
{
    ExamState *exam = &app->exam;
    int topics[TOPIC_COUNT];
    int available = AvailableTopics(app, topics);
    for (int i = available - 1; i > 0; i--) {   // Fisher-Yates shuffle
        int j = GetRandomValue(0, i);
        int temporary = topics[i];
        topics[i] = topics[j];
        topics[j] = temporary;
    }
    *exam = (ExamState){.phase = EXAM_QUESTION, .selected = NO_SELECTION};
    exam->count = available < EXAM_LENGTH ? available : EXAM_LENGTH;
    for (int i = 0; i < exam->count; i++) {
        exam->topics[i] = topics[i];
        if (topics[i] == ARRAYS_TOPIC) GenerateArraysQuestion(&exam->questions[i]);
        else exam->questions[i] = app->lessons[topics[i]].question;
        ShuffleOptions(&exam->questions[i]);
    }
}

static void DrawExamIntro(AppState *app)
{
    int topics[TOPIC_COUNT];
    int available = AvailableTopics(app, topics);
    Label("EXAM MODE", 40, 122, 18, CQ_PURPLE);
    Label("Test yourself", 40, 158, 36, CQ_TEXT);
    DrawPanel((Rectangle){40, 250, 1200, 376});
    if (available == 0) {
        Label("No lessons could be loaded, so there are no questions yet.", 68, 290, 24, CQ_TEXT);
        return;
    }
    Label(TextFormat("%d questions, each from a different topic, chosen at random.",
                     available < EXAM_LENGTH ? available : EXAM_LENGTH), 68, 290, 26, CQ_TEXT);
    Label("No hints and no second tries.", 68, 350, 24, CQ_MUTED);
    Label("Your score and the correct answers appear at the end.", 68, 390, 24, CQ_MUTED);
    Label("Exam answers do not change your badges.", 68, 430, 24, CQ_MUTED);
    if (Button((Rectangle){68, 544, 225, 52}, "Start exam", true)) StartExam(app);
}

static void DrawExamQuestion(AppState *app)
{
    ExamState *exam = &app->exam;
    const Question *question = &exam->questions[exam->index];
    bool last = exam->index + 1 == exam->count;
    Label(TextFormat("Exam / Question %d of %d", exam->index + 1, exam->count),
          40, 122, 18, CQ_PURPLE);
    Label(question->prompt, 40, 158, 36, CQ_TEXT);
    DrawPanel((Rectangle){40, 250, 570, 376});
    Label("C example", 68, 282, 26, CQ_TEXT);
    Label(question->code, 68, 345, 24, CQ_TEXT);
    DrawPanel((Rectangle){634, 250, 606, 376});
    Label("Choose one answer", 662, 282, 26, CQ_TEXT);
    for (int i = 0; i < OPTION_COUNT; i++) {
        const char *label = TextFormat("%c. %s", 'A' + i, question->options[i].code);
        if (Button((Rectangle){662, (float)(330 + 60*i), 550, 48}, label, exam->selected == i))
            exam->selected = i;
    }
    if (exam->selected == NO_SELECTION) {
        Label("Select an answer to continue.", 662, 557, 20, CQ_MUTED);
    } else if (Button((Rectangle){662, 544, 220, 52}, last ? "Finish exam" : "Next question", true)) {
        bool correct = IsCorrectAnswer(question, exam->selected);
        exam->correct[exam->index] = correct;
        if (correct) exam->score++;
        if (last) {
            exam->phase = EXAM_SUMMARY;
        } else {
            exam->index++;
            exam->selected = NO_SELECTION;
        }
    }
}

static void DrawExamSummary(AppState *app)
{
    const ExamState *exam = &app->exam;
    Label("Exam / Result", 40, 122, 18, CQ_PURPLE);
    Label(TextFormat("%d of %d correct", exam->score, exam->count), 40, 158, 36, CQ_TEXT);
    DrawPanel((Rectangle){40, 250, 1200, 376});
    for (int i = 0; i < exam->count; i++) {
        const Question *question = &exam->questions[i];
        int y = 276 + i * 52;
        Label(TextFormat("%d. %s", i + 1, topicTitles[exam->topics[i]]), 68, y, 22,
              exam->correct[i] ? CQ_TEXT : CQ_MUTED);
        if (exam->correct[i]) {
            Label("Correct", 420, y + 2, 20, CQ_PALE);
        } else {
            const char *answer = "";
            for (int option = 0; option < OPTION_COUNT; option++)
                if (IsCorrectAnswer(question, option)) answer = question->options[option].code;
            Label(TextFormat("Missed. Correct answer: %s", answer), 420, y + 2, 20, CQ_MUTED);
        }
    }
    if (Button((Rectangle){68, 544, 225, 52}, "Retake exam", true)) StartExam(app);
    if (Button((Rectangle){310, 544, 225, 52}, "Back to topic map", false)) app->screen = SCREEN_MAP;
}

static void DrawExam(AppState *app)
{
    switch (app->exam.phase) {
        case EXAM_INTRO: DrawExamIntro(app); break;
        case EXAM_QUESTION: DrawExamQuestion(app); break;
        case EXAM_SUMMARY: DrawExamSummary(app); break;
    }
}

static void DrawProgress(AppState *app)
{
    int badges = 0;
    Label("MY PROGRESS", 40, 122, 18, CQ_PURPLE);
    for (int i = 0; i < TOPIC_COUNT; i++) {
        const TopicProgress *topic = &app->progress.topics[i];
        bool hasTrace = i == ARRAYS_TOPIC || i == POINTERS_TOPIC || i == PARAMETERS_TOPIC;
        bool earned = topic->viewed && topic->solved && (!hasTrace || topic->traced);
        if (earned) badges++;
        int x = 40 + (i % 2)*610;
        int y = 258 + (i / 2)*85;
        DrawPanel((Rectangle){(float)x, (float)y, 590, 72});
        Label(topicTitles[i], x + 18, y + 9, 22, CQ_TEXT);
        const char *status = "Read the lesson";
        if (earned) status = "Badge earned";
        else if (topic->viewed && !topic->solved) status = "Pass the practice question";
        else if (topic->viewed) status = "Finish the walkthrough";
        Label(status, x + 18, y + 40, 18, earned ? CQ_PALE : CQ_MUTED);
        if (topic->firstTry || topic->persistent)
            Label(topic->firstTry && topic->persistent ? "First try  |  Persistent" :
                  topic->firstTry ? "First try" : "Persistent", x + 315, y + 40, 18, CQ_PALE);
    }
    Label(TextFormat("%d of %d topic badges earned%s", badges, TOPIC_COUNT,
                     badges == TOPIC_COUNT ? "  |  Master badge earned" : ""),
          40, 158, 36, CQ_TEXT);
    Label("Read the lesson, finish its walkthrough if available, and pass practice.",
          40, 211, 21, CQ_MUTED);
    if (app->progress.retry && Button((Rectangle){1030, 604, 210, 43}, "Retry save", true))
        SaveProgress(&app->progress);
}

void DrawApp(AppState *app)
{
    ClearBackground(CQ_BACKGROUND);
    DrawRectangle(0, 0, 1280, 88, CQ_SURFACE);
    Label("Code", 40, 23, 30, CQ_TEXT);
    Label("Quest", 40 + (int)TextWidth("Code", 30), 23, 30, CQ_PURPLE);
    if (Button((Rectangle){238, 20, 150, 48}, "Topic map", app->screen == SCREEN_MAP)) app->screen = SCREEN_MAP;
    if (Button((Rectangle){404, 20, 170, 48}, "My progress", app->screen == SCREEN_PROGRESS)) app->screen = SCREEN_PROGRESS;
    if (Button((Rectangle){590, 20, 120, 48}, "Exam", app->screen == SCREEN_EXAM)) {
        if (app->screen != SCREEN_EXAM) app->exam.phase = EXAM_INTRO;
        app->screen = SCREEN_EXAM;
    }
    switch (app->screen) {
        case SCREEN_MAP: DrawMap(app); break;
        case SCREEN_LESSON: DrawLesson(app); break;
        case SCREEN_PROGRESS: DrawProgress(app); break;
        case SCREEN_PRACTICE: DrawPractice(app); break;
        case SCREEN_RESULTS: DrawResults(app); break;
        case SCREEN_TRACE: DrawWalkthrough(app); break;
        case SCREEN_EXAM: DrawExam(app); break;
    }
    DrawRectangle(0, 656, 1280, 64, CQ_SURFACE);
    Label(app->progress.notice ? app->progress.notice : "CodeQuest", 40, 677, 17, CQ_MUTED);
}
