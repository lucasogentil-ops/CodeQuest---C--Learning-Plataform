#include "raylib.h"
#include "screens.h"
#include "ui.h"
#include <stdio.h>

enum { DESIGN_WIDTH = 1280, DESIGN_HEIGHT = 720 };

static Camera2D LayoutCamera(void)
{
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    float horizontal = (float)width / DESIGN_WIDTH;
    float vertical = (float)height / DESIGN_HEIGHT;
    float scale = horizontal < vertical ? horizontal : vertical;
    if (scale <= 0.0f) scale = 1.0f;

    int left = (int)((width - DESIGN_WIDTH * scale) / 2);
    int top = (int)((height - DESIGN_HEIGHT * scale) / 2);
    SetMouseOffset(-left, -top);
    SetMouseScale(1.0f / scale, 1.0f / scale);
    return (Camera2D){.offset = {(float)left, (float)top}, .zoom = scale};
}

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
    InitWindow(DESIGN_WIDTH, DESIGN_HEIGHT, "CodeQuest - Learn C step by step");
    if (!IsWindowReady()) return 1;
    SetWindowMinSize(640, 360);
    SetTargetFPS(60);
    if (!LoadUi()) {
        fprintf(stderr, "Cannot load assets/fonts/NotoSans.ttf. Rebuild to copy the font assets.\n");
        CloseWindow();
        return 1;
    }

    AppState app = {.screen = SCREEN_MAP, .selectedAnswer = NO_SELECTION};
    OpenProgress(&app.progress);
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
    bool fullscreen = false;
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F11)) {
            ToggleBorderlessWindowed();
            fullscreen = !fullscreen;
        }
        if (IsKeyPressed(KEY_F10)) {
            if (fullscreen) {
                ToggleBorderlessWindowed();
                fullscreen = false;
            }
            if (IsWindowMaximized()) RestoreWindow();
            else MaximizeWindow();
        }
        bool drawCursor = (fullscreen || IsWindowMaximized()) && IsCursorOnScreen() && IsWindowFocused();
        if (drawCursor && !IsCursorHidden()) HideCursor();
        if (!drawCursor && IsCursorHidden()) ShowCursor();
        Camera2D camera = LayoutCamera();
        BeginDrawing();
        BeginMode2D(camera);
        DrawApp(&app);
        EndMode2D();
        // Draw the pointer into the frame so recordings can capture it.
        if (drawCursor) {
            Vector2 tip = GetWorldToScreen2D(GetMousePosition(), camera);
            Vector2 bottom = {tip.x, tip.y + 21};
            Vector2 right = {tip.x + 15, tip.y + 14};
            DrawTriangle(tip, bottom, right, WHITE);
            DrawTriangleLines(tip, bottom, right, BLACK);
        }
        EndDrawing();
    }

    UnloadUi();
    CloseProgress();
    CloseWindow();
    return 0;
}
