#ifndef CODEQUEST_SCREENS_H
#define CODEQUEST_SCREENS_H

typedef enum { SCREEN_MAP, SCREEN_LESSON, SCREEN_PROGRESS } Screen;

typedef struct {
    Screen screen;
    int selectedTopic;
} AppState;

void DrawApp(AppState *app);

#endif
