#include "ui.h"
#include <stddef.h>

static Font uiFont;

bool LoadUi(void)
{
    /* Resolve assets from the executable, so launching from another folder works. */
    const char *path = TextFormat("%sassets/fonts/NotoSans.ttf", GetApplicationDirectory());
    if (!FileExists(path)) return false;
    uiFont = LoadFontEx(path, 64, NULL, 0);
    if (uiFont.texture.id == 0 || uiFont.texture.id == GetFontDefault().texture.id) return false;
    SetTextureFilter(uiFont.texture, TEXTURE_FILTER_BILINEAR);
    return true;
}

void UnloadUi(void) { UnloadFont(uiFont); }

float TextWidth(const char *text, int size)
{
    return MeasureTextEx(uiFont, text, (float)size, 0).x;
}

void Label(const char *text, int x, int y, int size, Color color)
{
    DrawTextEx(uiFont, text, (Vector2){(float)x, (float)y}, (float)size, 0, color);
}

void DrawPanel(Rectangle bounds)
{
    DrawRectangleRounded(bounds, 0.08f, 12, CQ_SURFACE);
}

/* Both button styles share the same press-and-release rule. */
static bool ButtonClicked(Rectangle bounds)
{
    static int pressedX = -1;
    static int pressedY = -1;
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, bounds);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        pressedX = (int)mouse.x;
        pressedY = (int)mouse.y;
    }
    bool startedHere = CheckCollisionPointRec((Vector2){(float)pressedX, (float)pressedY}, bounds);
    return hovered && startedHere && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

bool Button(Rectangle bounds, const char *label, bool primary)
{
    bool hovered = CheckCollisionPointRec(GetMousePosition(), bounds);
    Color fill = primary ? (hovered ? CQ_PALE : CQ_PURPLE) : (hovered ? CQ_BORDER : CQ_CARD);
    DrawRectangleRounded(bounds, 0.18f, 10, fill);
    if (!primary) DrawRectangleRoundedLinesEx(bounds, 0.18f, 10, 1, CQ_BORDER);
    float width = TextWidth(label, 20);
    Label(label, (int)(bounds.x + (bounds.width - width)/2),
          (int)(bounds.y + (bounds.height - 28)/2), 20, primary ? CQ_BACKGROUND : CQ_TEXT);
    return ButtonClicked(bounds);
}

bool TopicButton(Rectangle bounds, const char *title, int number)
{
    bool hovered = CheckCollisionPointRec(GetMousePosition(), bounds);
    DrawRectangleRounded(bounds, 0.13f, 12, hovered ? (Color){44, 59, 84, 255} : CQ_CARD);
    DrawRectangleRoundedLinesEx(bounds, 0.13f, 12, 1, CQ_BORDER);
    DrawRectangleRounded((Rectangle){bounds.x, bounds.y, 5, bounds.height}, 0.8f, 8, CQ_PALE);
    Label(TextFormat("%d. %s", number, title), (int)bounds.x + 24, (int)bounds.y + 16, 20, CQ_TEXT);
    Label("Ready to explore", (int)bounds.x + 24, (int)bounds.y + 47, 16, CQ_MUTED);
    return ButtonClicked(bounds);
}
