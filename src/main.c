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

    AppState app = {SCREEN_MAP, 0};
    while (!WindowShouldClose()) {
        BeginDrawing();
        DrawApp(&app);
        EndDrawing();
    }

    UnloadUi();
    CloseWindow();
    return 0;
}
