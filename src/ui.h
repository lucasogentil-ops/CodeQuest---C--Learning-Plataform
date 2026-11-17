#ifndef CODEQUEST_UI_H
#define CODEQUEST_UI_H

#include "raylib.h"

/* Palette. Surfaces go from darkest (background) to lightest (card). */
#define CQ_BACKGROUND ((Color){21, 29, 44, 255})
#define CQ_SURFACE ((Color){31, 42, 59, 255})
#define CQ_CARD ((Color){35, 48, 70, 255})
#define CQ_CARD_HOVER ((Color){44, 59, 84, 255})
#define CQ_BORDER ((Color){76, 90, 115, 255})
#define CQ_TEXT ((Color){237, 242, 248, 255})
#define CQ_MUTED ((Color){182, 194, 214, 255})
#define CQ_PURPLE ((Color){161, 123, 230, 255})
#define CQ_PALE ((Color){185, 154, 233, 255})
#define CQ_SHADOW ((Color){5, 9, 18, 110})
#define CQ_HEADER_TOP ((Color){39, 46, 74, 255})
/* Feedback colours, with a dark tint of each for filled backgrounds. */
#define CQ_SUCCESS ((Color){88, 204, 140, 255})
#define CQ_SUCCESS_DIM ((Color){30, 70, 58, 255})
#define CQ_ERROR ((Color){240, 110, 110, 255})
#define CQ_ERROR_DIM ((Color){80, 38, 48, 255})
#define CQ_WARNING ((Color){242, 186, 84, 255})
#define CQ_WARNING_DIM ((Color){78, 60, 34, 255})
/* Code colours. */
#define CQ_CODE_BG ((Color){17, 23, 36, 255})

bool LoadUi(void);
void UnloadUi(void);

/* Text in the UI font. */
float TextWidth(const char *text, int size);
void Label(const char *text, int x, int y, int size, Color color);
/* Shrinks the size (down to 60%) until the text fits on one line, keeping it vertically centred. */
void LabelFit(const char *text, int x, int y, int maxWidth, int size, Color color);
/* Word-wraps at maxWidth and honours '\n'. Returns the height used. */
float LabelWrapped(const char *text, int x, int y, int maxWidth, int size, Color color);

/* Code in the monospace font. */
float CodeWidth(const char *text, int size);
void CodeLabel(const char *text, int x, int y, int size, Color color);
/* Largest size up to maxSize (min 12) at which every line fits in width x height. */
int CodeFitSize(const char *code, float width, float height, int maxSize);
/* Draws multi-line code on a dark inset, fitted to bounds. Returns the size used. */
int DrawCodeBlock(const char *code, Rectangle bounds, int maxSize);

/* Layout: carve rectangles from an area, "RectCut" style. */
Rectangle RectInset(Rectangle r, float amount);
Rectangle CutTop(Rectangle *area, float height);
Rectangle CutBottom(Rectangle *area, float height);
Rectangle CutLeft(Rectangle *area, float width);
Rectangle CutRight(Rectangle *area, float width);

/* Widgets. */
void DrawPanel(Rectangle bounds);
bool Button(Rectangle bounds, const char *label, bool primary);
/* An answer choice: letter chip plus code in the monospace font. */
bool OptionButton(Rectangle bounds, int index, const char *code, bool selected);
bool TopicButton(Rectangle bounds, const char *title, int number);
void DrawHeaderBar(float width, float height);
/* Medal icon: tier 0 draws an empty slot, 1-3 draw bronze, silver and gold with 1-3 stars. */
void DrawBadgeIcon(Vector2 center, float radius, int tier);

#endif
