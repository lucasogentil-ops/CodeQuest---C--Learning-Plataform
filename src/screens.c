#include "screens.h"
#include "ui.h"
#include <stddef.h>

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
        if (IsCorrectAnswer(question, app->selectedAnswer))
            app->practicePassed[app->selectedTopic] = true;
        else if (app->wrongAttempts < 2)
            app->wrongAttempts++;
        app->screen = SCREEN_RESULTS;
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
        if (correct || app->showingSolution) app->wrongAttempts = 0;
        app->showingSolution = false;
        app->screen = SCREEN_PRACTICE;
    }
    if (!correct && app->wrongAttempts >= 2 && !app->showingSolution &&
        Button((Rectangle){532, 544, 220, 52}, "Show solution", false))
        app->showingSolution = true;
    if (app->selectedTopic == ARRAYS_TOPIC &&
        Button((Rectangle){770, 544, 220, 52}, "New question", false))
        StartNewQuestion(app);
}

static void DrawProgress(const AppState *app)
{
    int passed = 0;
    for (int i = 0; i < TOPIC_COUNT; i++)
        if (app->practicePassed[i]) passed++;
    Label("MY PROGRESS", 40, 122, 18, CQ_PURPLE);
    Label("Your learning starts here", 40, 158, 36, CQ_TEXT);
    Label(TextFormat("Practice passed this session: %d of %d topics", passed, TOPIC_COUNT),
          40, 216, 24, CQ_MUTED);
    DrawPanel((Rectangle){40, 286, 1200, 260});
    Label("Progress and badges are coming soon", 70, 320, 28, CQ_TEXT);
    Label("Complete lessons and answer questions to earn topic badges.", 70, 384, 24, CQ_TEXT);
    Label("Local saving will be added in the progress milestone.", 70, 444, 22, CQ_MUTED);
}

void DrawApp(AppState *app)
{
    ClearBackground(CQ_BACKGROUND);
    DrawRectangle(0, 0, 1280, 88, CQ_SURFACE);
    Label("Code", 40, 23, 30, CQ_TEXT);
    Label("Quest", 40 + (int)TextWidth("Code", 30), 23, 30, CQ_PURPLE);
    if (Button((Rectangle){238, 20, 150, 48}, "Topic map", app->screen == SCREEN_MAP)) app->screen = SCREEN_MAP;
    if (Button((Rectangle){404, 20, 170, 48}, "My progress", app->screen == SCREEN_PROGRESS)) app->screen = SCREEN_PROGRESS;
    switch (app->screen) {
        case SCREEN_MAP: DrawMap(app); break;
        case SCREEN_LESSON: DrawLesson(app); break;
        case SCREEN_PROGRESS: DrawProgress(app); break;
        case SCREEN_PRACTICE: DrawPractice(app); break;
        case SCREEN_RESULTS: DrawResults(app); break;
        case SCREEN_TRACE: DrawWalkthrough(app); break;
    }
    DrawRectangle(0, 656, 1280, 64, CQ_SURFACE);
    Label("CodeQuest  |  Development build", 40, 677, 17, CQ_MUTED);
}
