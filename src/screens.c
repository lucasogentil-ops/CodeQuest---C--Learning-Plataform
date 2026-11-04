#include "screens.h"
#include "ui.h"

static const char *topicTitles[] = {
    "C foundations", "Decisions and loops", "Arrays", "2D arrays",
    "Strings and scope", "Functions and parameters", "Pointers", "Linked lists"
};

static const char *topicDescriptions[] = {
    "Types, input/output and operators", "Conditions, repetition and patterns",
    "Search, update, reverse and rotate", "Explore values in rows and columns",
    "Characters, strings and visibility", "Return values and passing arguments",
    "Addresses and changing pointed-to values", "Nodes, links and insertion"
};

static void DrawMap(AppState *app)
{
    Label("YOUR LEARNING PATH", 40, 122, 18, CQ_PURPLE);
    Label("Small steps. Clearer code.", 40, 158, 36, CQ_TEXT);
    Label("Choose any topic to explore. Every topic stays available.", 40, 210, 20, CQ_MUTED);

    for (int i = 0; i < 8; i++) {
        int x = 40 + (i%2)*392;
        int y = 270 + (i/2)*92;
        if (TopicButton((Rectangle){(float)x, (float)y, 372, 80}, topicTitles[i], i + 1)) {
            app->selectedTopic = i;
            app->screen = SCREEN_LESSON;
        }
    }
    DrawPanel((Rectangle){850, 270, 390, 356});
    Label("YOUR FIRST STEP", 878, 296, 16, CQ_PALE);
    Label("Start with C foundations", 878, 332, 25, CQ_TEXT);
    Label("Short lessons. Visual examples.", 878, 380, 20, CQ_MUTED);
    Label("Practice at your own pace.", 878, 410, 20, CQ_MUTED);
    if (Button((Rectangle){878, 466, 334, 50}, "Explore C foundations", true)) {
        app->selectedTopic = 0;
        app->screen = SCREEN_LESSON;
    }
    Label("Your badges", 878, 545, 21, CQ_TEXT);
    Label("Your first badge is ahead of you.", 878, 581, 18, CQ_MUTED);
}

static void DrawLesson(AppState *app)
{
    int topic = app->selectedTopic;
    Label("TOPIC PREVIEW", 40, 122, 18, CQ_PURPLE);
    Label(topicTitles[topic], 40, 158, 36, CQ_TEXT);
    Label(topicDescriptions[topic], 40, 210, 20, CQ_MUTED);
    DrawPanel((Rectangle){40, 270, 720, 350});
    Label("A clearer way to revise", 68, 300, 28, CQ_TEXT);
    Label("This topic will include a short explanation,", 68, 356, 24, CQ_TEXT);
    Label("a labelled code example and a practice question.", 68, 396, 24, CQ_TEXT);
    Label("Lesson content is coming in the next milestone.", 68, 472, 22, CQ_MUTED);
    DrawPanel((Rectangle){790, 270, 450, 350});
    Label("Learning journey", 818, 300, 26, CQ_TEXT);
    Label("Read the lesson", 818, 364, 22, CQ_TEXT);
    Label("Step through an example", 818, 410, 22, CQ_TEXT);
    Label("Try a missing-code question", 818, 456, 22, CQ_TEXT);
    if (Button((Rectangle){818, 532, 394, 52}, "Back to topic map", true)) app->screen = SCREEN_MAP;
}

static void DrawProgress(void)
{
    Label("MY PROGRESS", 40, 122, 18, CQ_PURPLE);
    Label("Your learning starts here", 40, 158, 36, CQ_TEXT);
    Label("No completed topics yet.", 40, 216, 24, CQ_MUTED);
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
        case SCREEN_PROGRESS: DrawProgress(); break;
    }
    DrawRectangle(0, 656, 1280, 64, CQ_SURFACE);
    Label("CodeQuest  |  Development build - navigation preview", 40, 677, 17, CQ_MUTED);
}
