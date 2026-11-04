#ifndef CODEQUEST_UI_H
#define CODEQUEST_UI_H

#include "raylib.h"

#define CQ_BACKGROUND ((Color){21, 29, 44, 255})
#define CQ_SURFACE ((Color){31, 42, 59, 255})
#define CQ_CARD ((Color){35, 48, 70, 255})
#define CQ_BORDER ((Color){76, 90, 115, 255})
#define CQ_TEXT ((Color){237, 242, 248, 255})
#define CQ_MUTED ((Color){182, 194, 214, 255})
#define CQ_PURPLE ((Color){161, 123, 230, 255})
#define CQ_PALE ((Color){185, 154, 233, 255})

bool LoadUi(void);
void UnloadUi(void);
float TextWidth(const char *text, int size);
void DrawPanel(Rectangle bounds);
bool Button(Rectangle bounds, const char *label, bool primary);
bool TopicButton(Rectangle bounds, const char *title, int number);
void Label(const char *text, int x, int y, int size, Color color);

#endif
