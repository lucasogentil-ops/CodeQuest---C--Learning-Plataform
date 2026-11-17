#include "ui.h"
#include <math.h>
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

static void DrawStar(Vector2 center, float radius, Color color)
{
    Vector2 points[10];
    for (int i = 0; i < 10; i++) {
        float angle = (-90.0f + 36.0f * i) * DEG2RAD;
        float r = i % 2 ? radius * 0.45f : radius;
        points[i] = (Vector2){center.x + cosf(angle) * r, center.y + sinf(angle) * r};
    }
    // Angles grow clockwise on screen, so list each triangle backwards to keep it counter-clockwise.
    for (int i = 0; i < 10; i++) DrawTriangle(center, points[(i + 1) % 10], points[i], color);
}

void DrawBadgeIcon(Vector2 center, float radius, int tier)
{
    if (tier <= 0) {
        DrawCircleV(center, radius, CQ_CARD);
        DrawRing(center, radius - 2, radius, 0, 360, 48, CQ_BORDER);
        DrawStar(center, radius * 0.45f, CQ_BORDER);
        return;
    }
    static const Color metal[] = {{205, 127, 50, 255}, {196, 206, 222, 255}, {242, 194, 58, 255}};
    static const Color shade[] = {{140, 82, 32, 255}, {120, 132, 152, 255}, {176, 128, 20, 255}};
    Color face = metal[tier - 1], edge = shade[tier - 1];

    // Ribbon tails behind the medal.
    float w = radius * 0.5f, length = radius * 1.25f, top = center.y + radius * 0.2f;
    DrawRectanglePro((Rectangle){center.x - radius * 0.3f, top, w, length}, (Vector2){w / 2, 0},
                     22, CQ_PURPLE);
    DrawRectanglePro((Rectangle){center.x + radius * 0.3f, top, w, length}, (Vector2){w / 2, 0},
                     -22, CQ_PALE);

    DrawCircleV(center, radius, edge);
    DrawCircleV(center, radius * 0.82f, face);
    DrawRing(center, radius * 0.66f, radius * 0.7f, 0, 360, 48, edge);

    if (tier == 1) {
        DrawStar(center, radius * 0.5f, edge);
    } else if (tier == 2) {
        DrawStar((Vector2){center.x - radius * 0.3f, center.y}, radius * 0.32f, edge);
        DrawStar((Vector2){center.x + radius * 0.3f, center.y}, radius * 0.32f, edge);
    } else {
        DrawStar((Vector2){center.x, center.y - radius * 0.22f}, radius * 0.3f, edge);
        DrawStar((Vector2){center.x - radius * 0.33f, center.y + radius * 0.2f}, radius * 0.26f, edge);
        DrawStar((Vector2){center.x + radius * 0.33f, center.y + radius * 0.2f}, radius * 0.26f, edge);
    }
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
