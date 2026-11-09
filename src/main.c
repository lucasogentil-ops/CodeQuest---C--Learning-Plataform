#include "raylib.h"
#include "screens.h"
#include "ui.h"
#include <stdio.h>

int main(void)
{
    InitWindow(1280, 720, "CodeQuest - Learn C step by step");
    if (!IsWindowReady()) return 1;
    SetTargetFPS(60);
    if (!LoadUi()) {
        fprintf(stderr, "Cannot load assets/fonts/NotoSans.ttf. Rebuild to copy the font assets.\n");
        CloseWindow();
        return 1;
    }

    AppState app = {.screen = SCREEN_MAP, .selectedAnswer = NO_SELECTION};
    const char *lessonFiles[TOPIC_COUNT] = {
        [FOUNDATIONS_TOPIC] = "foundations.json", [LOOPS_TOPIC] = "loops.json",
        [ARRAYS_TOPIC] = "arrays.json", [GRID_TOPIC] = "arrays-2d.json",
        [STRINGS_TOPIC] = "strings.json", [PARAMETERS_TOPIC] = "functions.json",
        [POINTERS_TOPIC] = "pointers.json", [LISTS_TOPIC] = "linked-lists.json"
    };
    for (int i = 0; i < TOPIC_COUNT; i++) {
        const char *path = TextFormat("%scontent/%s", GetApplicationDirectory(), lessonFiles[i]);
        app.lessonAvailable[i] = LoadLesson(path, &app.lessons[i]);
        if (!app.lessonAvailable[i])
            fprintf(stderr, "Cannot load lesson (missing file or invalid content): %s\n", path);
    }
    const char *traceFiles[TRACE_COUNT] = {
        [ARRAYS_TRACE] = "trace-arrays.json",
        [POINTERS_TRACE] = "trace-pointers.json",
        [PARAMETERS_TRACE] = "trace-parameters.json"
    };
    for (int i = 0; i < TRACE_COUNT; i++) {
        const char *path = TextFormat("%scontent/%s", GetApplicationDirectory(), traceFiles[i]);
        if (!LoadWalkthrough(path, &app.traces[i]))
            fprintf(stderr, "Cannot load walkthrough: %s\n", path);
    }
    while (!WindowShouldClose()) {
        BeginDrawing();
        DrawApp(&app);
        EndDrawing();
    }

    UnloadUi();
    CloseWindow();
    return 0;
}
