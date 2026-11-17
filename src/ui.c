#include "ui.h"
#include "anim.h"
#include <math.h>
#include <stddef.h>
#include <string.h>

static Font uiFont;
static Font codeFont;
static float codeAdvance;   /* width of one monospace character at size 1 */

static bool LoadAppFont(const char *file, Font *font)
{
    /* Resolve assets from the executable, so launching from another folder works. */
    const char *path = TextFormat("%sassets/fonts/%s", GetApplicationDirectory(), file);
    if (!FileExists(path)) return false;
    *font = LoadFontEx(path, 64, NULL, 0);
    if (font->texture.id == 0 || font->texture.id == GetFontDefault().texture.id) return false;
    SetTextureFilter(font->texture, TEXTURE_FILTER_BILINEAR);
    return true;
}

bool LoadUi(void)
{
    if (!LoadAppFont("NotoSans.ttf", &uiFont)) return false;
    if (!LoadAppFont("NotoSansMono.ttf", &codeFont)) {
        UnloadFont(uiFont);
        return false;
    }
    codeAdvance = MeasureTextEx(codeFont, "M", 100, 0).x / 100;
    return true;
}

void UnloadUi(void)
{
    UnloadFont(uiFont);
    UnloadFont(codeFont);
}

/* ---------- Text ---------- */

float TextWidth(const char *text, int size)
{
    return MeasureTextEx(uiFont, text, (float)size, 0).x;
}

void Label(const char *text, int x, int y, int size, Color color)
{
    DrawTextEx(uiFont, text, (Vector2){(float)x, (float)y}, (float)size, 0, color);
}

void LabelFit(const char *text, int x, int y, int maxWidth, int size, Color color)
{
    int fitted = size;
    while (fitted > size * 3 / 5 && TextWidth(text, fitted) > maxWidth) fitted--;
    Label(text, x, y + (size - fitted) / 2, fitted, color);
}

/* Copies text[0..length) into a buffer so raylib can measure or draw just that slice. */
static const char *Slice(const char *text, size_t length)
{
    static char buffer[512];
    if (length >= sizeof buffer) length = sizeof buffer - 1;
    memcpy(buffer, text, length);
    buffer[length] = '\0';
    return buffer;
}

float LabelWrapped(const char *text, int x, int y, int maxWidth, int size, Color color)
{
    float lineHeight = size * 1.3f;
    float cursor = (float)y;
    const char *line = text;
    while (*line) {
        size_t paragraph = strcspn(line, "\n");
        const char *start = line, *end = line + paragraph;
        while (start < end) {
            // Grow the line word by word until the next word would overflow.
            const char *fit = NULL, *scan = start;
            while (scan < end) {
                const char *wordEnd = scan;
                while (wordEnd < end && *wordEnd != ' ') wordEnd++;
                if (fit && TextWidth(Slice(start, wordEnd - start), size) > maxWidth) break;
                fit = wordEnd;
                scan = wordEnd < end ? wordEnd + 1 : end;
            }
            Label(Slice(start, fit - start), x, (int)cursor, size, color);
            cursor += lineHeight;
            start = fit;
            while (start < end && *start == ' ') start++;
        }
        if (paragraph == 0) cursor += lineHeight;   // blank line
        line = *end ? end + 1 : end;
    }
    return cursor - y;
}

float CodeWidth(const char *text, int size)
{
    return MeasureTextEx(codeFont, text, (float)size, 0).x;
}

void CodeLabel(const char *text, int x, int y, int size, Color color)
{
    DrawTextEx(codeFont, text, (Vector2){(float)x, (float)y}, (float)size, 0, color);
}

static const float CODE_LINE_HEIGHT = 1.45f;

int CodeFitSize(const char *code, float width, float height, int maxSize)
{
    int lines = 1, longest = 0, current = 0;
    for (const char *c = code; *c; c++) {
        if (*c == '\n') { lines++; current = 0; continue; }
        if (++current > longest) longest = current;
    }
    float size = (float)maxSize;
    if (longest > 0 && longest * codeAdvance * size > width) size = width / (longest * codeAdvance);
    if (lines * CODE_LINE_HEIGHT * size > height) size = height / (lines * CODE_LINE_HEIGHT);
    return size < 12 ? 12 : (int)size;
}

int DrawCodeBlock(const char *code, Rectangle bounds, int maxSize)
{
    DrawRectangleRounded(bounds, 0.06f, 10, CQ_CODE_BG);
    Rectangle inner = RectInset(bounds, 16);
    int size = CodeFitSize(code, inner.width, inner.height, maxSize);
    float y = inner.y;
    const char *line = code;
    while (*line) {
        size_t length = strcspn(line, "\n");
        CodeLabel(Slice(line, length), (int)inner.x, (int)y, size, CQ_TEXT);
        y += size * CODE_LINE_HEIGHT;
        line += length;
        if (*line) line++;
    }
    return size;
}

/* ---------- Layout ---------- */

Rectangle RectInset(Rectangle r, float amount)
{
    return (Rectangle){r.x + amount, r.y + amount, r.width - 2 * amount, r.height - 2 * amount};
}

Rectangle CutTop(Rectangle *area, float height)
{
    Rectangle cut = {area->x, area->y, area->width, height};
    area->y += height;
    area->height -= height;
    return cut;
}

Rectangle CutBottom(Rectangle *area, float height)
{
    area->height -= height;
    return (Rectangle){area->x, area->y + area->height, area->width, height};
}

Rectangle CutLeft(Rectangle *area, float width)
{
    Rectangle cut = {area->x, area->y, width, area->height};
    area->x += width;
    area->width -= width;
    return cut;
}

Rectangle CutRight(Rectangle *area, float width)
{
    area->width -= width;
    return (Rectangle){area->x + area->width, area->y, width, area->height};
}

/* ---------- Widgets ---------- */

void DrawPanel(Rectangle bounds)
{
    DrawRectangleRounded((Rectangle){bounds.x, bounds.y + 4, bounds.width, bounds.height},
                         0.08f, 12, CQ_SHADOW);
    DrawRectangleRounded(bounds, 0.08f, 12, CQ_SURFACE);
}

void DrawHeaderBar(float width, float height)
{
    DrawRectangleGradientV(0, 0, (int)width, (int)height, CQ_HEADER_TOP, CQ_SURFACE);
    DrawRectangleGradientV(0, (int)height, (int)width, 6, CQ_SHADOW, (Color){0, 0, 0, 0});
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

/* Animated hover amount for a widget, 0 at rest and 1 fully hovered. */
static float HoverAmount(Rectangle bounds)
{
    bool hovered = CheckCollisionPointRec(GetMousePosition(), bounds);
    return AnimValue(AnimIdRect(bounds), hovered ? 1.0f : 0.0f, 16);
}

/* Pressing shrinks a widget slightly around its centre. */
static Rectangle PressedBounds(Rectangle bounds)
{
    bool pressed = IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), bounds);
    float press = AnimValue(AnimIdRect(bounds) ^ 0x9e3779b9u, pressed ? 1.0f : 0.0f, 30);
    return RectInset(bounds, press * 2);
}

bool Button(Rectangle bounds, const char *label, bool primary)
{
    float hover = HoverAmount(bounds);
    Rectangle shape = PressedBounds(bounds);
    Color fill = primary ? ColorLerp(CQ_PURPLE, CQ_PALE, hover) : ColorLerp(CQ_CARD, CQ_BORDER, hover);
    DrawRectangleRounded(shape, 0.18f, 10, fill);
    if (!primary) DrawRectangleRoundedLinesEx(shape, 0.18f, 10, 1, CQ_BORDER);
    float width = TextWidth(label, 20);
    Label(label, (int)(bounds.x + (bounds.width - width)/2),
          (int)(bounds.y + (bounds.height - 28)/2), 20, primary ? CQ_BACKGROUND : CQ_TEXT);
    return ButtonClicked(bounds);
}

bool OptionButton(Rectangle bounds, int index, const char *code, bool selected)
{
    float hover = HoverAmount(bounds);
    Rectangle shape = PressedBounds(bounds);
    Color fill = selected ? CQ_PURPLE : ColorLerp(CQ_CARD, CQ_CARD_HOVER, hover);
    Color text = selected ? CQ_BACKGROUND : CQ_TEXT;
    DrawRectangleRounded(shape, 0.18f, 10, fill);
    if (!selected) DrawRectangleRoundedLinesEx(shape, 0.18f, 10, 1, ColorLerp(CQ_BORDER, CQ_PALE, hover));

    Rectangle area = bounds;
    Rectangle chip = RectInset(CutLeft(&area, bounds.height), 9);
    DrawRectangleRounded(chip, 0.35f, 8, selected ? CQ_BACKGROUND : CQ_SURFACE);
    char letter[2] = {(char)('A' + index), '\0'};
    Label(letter, (int)(chip.x + (chip.width - TextWidth(letter, 18)) / 2),
          (int)(chip.y + (chip.height - 25) / 2), 18, selected ? CQ_PALE : CQ_MUTED);

    area = RectInset(area, 4);
    int size = CodeFitSize(code, area.width - 8, area.height, 20);
    CodeLabel(code, (int)area.x + 4, (int)(area.y + (area.height - size * 1.35f) / 2), size, text);
    return ButtonClicked(bounds);
}

bool TopicButton(Rectangle bounds, const char *title, int number)
{
    float hover = HoverAmount(bounds);
    Rectangle shape = bounds;
    shape.y -= 3 * hover;    // lifts toward the pointer
    DrawRectangleRounded((Rectangle){shape.x, bounds.y + 3 + hover * 2, shape.width, shape.height},
                         0.13f, 12, Fade(CQ_SHADOW, 0.4f + 0.6f * hover));
    DrawRectangleRounded(shape, 0.13f, 12, ColorLerp(CQ_CARD, CQ_CARD_HOVER, hover));
    DrawRectangleRoundedLinesEx(shape, 0.13f, 12, 1, ColorLerp(CQ_BORDER, CQ_PALE, hover * 0.6f));
    DrawRectangleRounded((Rectangle){shape.x, shape.y, 5, shape.height}, 0.8f, 8, CQ_PALE);
    Label(TextFormat("%d. %s", number, title), (int)shape.x + 24, (int)shape.y + 16, 20, CQ_TEXT);
    Label("Ready to explore", (int)shape.x + 24, (int)shape.y + 47, 16, CQ_MUTED);
    return ButtonClicked(bounds);
}

/* ---------- Badges ---------- */

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
