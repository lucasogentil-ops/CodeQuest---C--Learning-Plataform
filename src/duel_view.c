#include "duel.h"
#include "ui.h"
#include <math.h>

static Texture2D arena;
typedef struct {
    Texture2D texture;
    Rectangle source;
} FighterSprite;

static FighterSprite bytefox;
static FighterSprite nullWarden;
static float animationTime;
static bool shieldBeforeAnswer;
static const float animationLength = 1.45f;
static const Vector2 fox = {343, 338};
static const Vector2 warden = {886, 219};
static const Color mint = {134, 244, 200, 255};
static const Color manaColor = {116, 189, 255, 255};
static const Color danger = {250, 115, 140, 255};
static const char *moves[] = {"Strike", "Guard", "Break Shield"};

static FighterSprite LoadFighter(const char *file)
{
    Image image = LoadImage(TextFormat("%sassets/duel/%s", GetApplicationDirectory(), file));
    if (!image.data) return (FighterSprite){0};
    FighterSprite sprite = {LoadTextureFromImage(image), GetImageAlphaBorder(image, 0.12f)};
    UnloadImage(image);
    return sprite;
}

void LoadDuelArt(void)
{
    arena = LoadTexture(TextFormat("%sassets/duel/observatory.png", GetApplicationDirectory()));
    bytefox = LoadFighter("bytefox.png");
    nullWarden = LoadFighter("null-warden.png");
}

void UnloadDuelArt(void)
{
    if (bytefox.texture.id) UnloadTexture(bytefox.texture);
    if (nullWarden.texture.id) UnloadTexture(nullWarden.texture);
    if (arena.id) UnloadTexture(arena);
}

static float ProgressBetween(float time, float start, float end)
{
    if (time <= start) return 0;
    if (time >= end) return 1;
    return (time - start) / (end - start);
}

static float MotionPulse(float time, float start, float peak, float end)
{
    return ProgressBetween(time, start, peak) * (1 - ProgressBetween(time, peak, end));
}

static void DrawFighter(FighterSprite sprite, Vector2 feet, float width, float scaleY, float tilt, Color tint)
{
    if (!sprite.texture.id || sprite.source.width <= 0) return;
    float height = width * sprite.source.height / sprite.source.width * scaleY;
    Rectangle bounds = {feet.x, feet.y, width, height};
    DrawTexturePro(sprite.texture, sprite.source, bounds, (Vector2){width / 2, height}, tilt, tint);
}

static void DrawFighters(const DuelState *duel)
{
    const int idle[] = {0, 0, -2, -4, -4, -2};
    int frame = (int)(GetTime() * 6) % 6;
    Vector2 foxFeet = {343, 424 + idle[frame] / 2};
    Vector2 bossFeet = {886, 317 + idle[(frame + 2) % 6] * 2};
    float foxScale = 1;
    float foxTilt = 0;
    float bossTilt = 0;
    Color foxTint = WHITE;
    Color bossTint = WHITE;

    if (duel->phase == DUEL_FEEDBACK) {
        /* Hold each pose briefly to match the stepped idle loop. */
        float time = floorf(animationTime * 18) / 18;
        bool guard = duel->correct && duel->move == DUEL_GUARD;
        float bossHit = guard ? 0.82f : 0.36f;
        float foxHit = guard ? 0.35f : 0.90f;
        if (guard) {
            foxScale -= 0.19f * MotionPulse(time, 0.09f, 0.35f, 0.58f);
        }
        if (duel->damage > 0) {
            float lunge = MotionPulse(time, bossHit - 0.30f, bossHit - 0.13f, bossHit + 0.08f);
            foxFeet.x += roundf(62 * lunge);
            foxFeet.y -= roundf(23 * lunge);
            foxTilt += 9 * lunge;
            float recoil = MotionPulse(time, bossHit, bossHit + 0.09f, bossHit + 0.32f);
            bossFeet.x += roundf(25 * recoil);
            bossFeet.y -= roundf(13 * recoil);
            bossTilt += 8 * recoil;
            if (recoil > 0.45f) bossTint = (Color){255, 167, 198, 255};
        }
        if (duel->incoming > 0 || guard) {
            float swoop = MotionPulse(time, foxHit - 0.32f, foxHit - 0.12f, foxHit + 0.16f);
            bossFeet.x -= roundf(46 * swoop);
            bossFeet.y += roundf(33 * swoop);
            bossTilt -= 10 * swoop;
        }
        if (duel->incoming > 0) {
            float recoil = MotionPulse(time, foxHit, foxHit + 0.1f, foxHit + 0.34f);
            foxFeet.x -= roundf(24 * recoil);
            foxFeet.y -= roundf(12 * recoil);
            foxTilt -= 12 * recoil;
            if (recoil > 0.45f) foxTint = (Color){255, 157, 172, 255};
        }
        if (duel->bossHp == 0)
            bossTint = Fade(bossTint, 1 - ProgressBetween(time, bossHit + 0.12f, 1.25f));
        if (duel->playerHp == 0)
            foxTint = Fade(foxTint, 1 - ProgressBetween(time, foxHit + 0.08f, 1.40f));
    }
    if (duel->phase != DUEL_LOST) {
        DrawEllipse((int)foxFeet.x, 426, 74, 10, Fade(BLACK, 0.34f * foxTint.a / 255));
        DrawFighter(bytefox, foxFeet, 201, foxScale, foxTilt, foxTint);
    }
    if (duel->phase != DUEL_WON) {
        DrawEllipse((int)bossFeet.x, 363, 116, 10, Fade(BLACK, 0.23f * bossTint.a / 255));
        DrawFighter(nullWarden, bossFeet, 325, 1, bossTilt, bossTint);
    }
}

static void DrawProjectile(Vector2 from, Vector2 to, float progress, Color color)
{
    Vector2 tip = {from.x + (to.x - from.x) * progress,
                   from.y + (to.y - from.y) * progress};
    Vector2 tail = {tip.x - (to.x - from.x) * 0.12f,
                    tip.y - (to.y - from.y) * 0.12f};
    DrawLineEx(tail, tip, 8, Fade(color, 0.55f));
    DrawCircleV(tip, 11, Fade(color, 0.3f));
    DrawCircleV(tip, 5, color);
}

static void DrawImpact(Vector2 center, float progress, Color color, bool shatter)
{
    Color fading = Fade(color, 1 - progress);
    int radius = (int)(12 + progress * (shatter ? 128 : 62));
    DrawCircleLines((int)center.x, (int)center.y, (float)radius, fading);
    for (int i = 0; i < 12; i++) {
        float angle = i * 2 * PI / 12;
        Vector2 particle = {center.x + cosf(angle) * radius, center.y + sinf(angle) * radius};
        DrawCircleV(particle, shatter ? 5 : 3, fading);
    }
}

static void DrawDamage(Vector2 center, int damage, float progress, Color color)
{
    Label(TextFormat("-%d HP", damage), (int)center.x + 24,
          (int)(center.y - 20 - progress * 42), 25, Fade(color, 1 - progress));
}

static void DrawBattleEffects(const DuelState *duel, float time)
{
    bool guarded = duel->correct && duel->move == DUEL_GUARD;
    float bossHit = guarded ? 0.82f : 0.36f;
    float foxHit = guarded ? 0.35f : 0.90f;
    if ((duel->incoming > 0 || guarded) && time > foxHit - 0.29f && time < foxHit)
        DrawProjectile(warden, fox, ProgressBetween(time, foxHit - 0.29f, foxHit), danger);
    if (duel->damage > 0 && time > bossHit - 0.29f && time < bossHit)
        DrawProjectile(fox, warden, ProgressBetween(time, bossHit - 0.29f, bossHit), mint);
    if (guarded && time > foxHit - 0.12f && time < foxHit + 0.32f) {
        DrawCircleV(fox, 53, Fade(mint, 0.15f));
        DrawCircleLines((int)fox.x, (int)fox.y, 53, mint);
        Label("BLOCKED", (int)fox.x - 48, (int)fox.y - 79, 22, mint);
    }

    float hit = ProgressBetween(time, bossHit, bossHit + 0.51f);
    if (duel->damage > 0 && hit > 0 && hit < 1) {
        DrawImpact(warden, hit, mint, shieldBeforeAnswer && !duel->shield);
        DrawDamage(warden, duel->damage, hit, mint);
    }
    hit = ProgressBetween(time, foxHit, foxHit + 0.48f);
    if (duel->incoming > 0 && hit > 0 && hit < 1) {
        DrawImpact(fox, hit, danger, false);
        DrawDamage(fox, duel->incoming, hit, danger);
    }
}

static void DrawHealth(int x, int y, const char *name, float hp, int maxHp, Color color)
{
    DrawPanel((Rectangle){(float)x, (float)y, 300, 72});
    Label(name, x + 14, y + 8, 21, CQ_TEXT);
    Label(TextFormat("%d / %d HP", (int)ceilf(hp), maxHp), x + 194, y + 11, 17, CQ_MUTED);
    DrawRectangle(x + 14, y + 48, 272, 9, CQ_BORDER);
    DrawRectangle(x + 14, y + 48, (int)(272 * hp / maxHp), 9, color);
}

static void DrawFog(void)
{
    float seconds = (float)GetTime();
    for (int i = 0; i < 12; i++) {
        float x = 140 + fmodf(seconds * (12 + i % 3 * 4) + i * 131, 1000);
        float y = 331 + (i % 4) * 17 + 8 * sinf(seconds * 0.7f + i);
        float edge = fminf((x - 140) / 120, (1140 - x) / 120);
        Color mist = Fade(i % 2 ? manaColor : CQ_PALE, 0.16f * fminf(edge, 1));
        DrawCircleGradient((Vector2){x, y}, 75 + (i % 3) * 12, mist, Fade(mist, 0));
    }
}

static void DrawArena(const DuelState *duel)
{
    Rectangle bounds = {40, 102, 1200, 394};
    DrawRectangleRec(bounds, CQ_CARD);
    if (arena.id) {
        DrawTexturePro(arena, (Rectangle){0, 0, (float)arena.width, (float)arena.height},
                       bounds, (Vector2){0}, 0, WHITE);
    }
    DrawFog();
    DrawFighters(duel);
    bool feedback = duel->phase == DUEL_FEEDBACK;
    bool guarded = duel->correct && duel->move == DUEL_GUARD;
    float bossHit = guarded ? 0.82f : 0.36f;
    float foxHit = guarded ? 0.35f : 0.90f;
    bool shield = feedback && animationTime < bossHit ? shieldBeforeAnswer : duel->shield;
    float pulse = (float)(0.5 + 0.5 * sin(GetTime() * 3));
    if (shield || duel->exposed) {
        Color color = shield ? CQ_PALE : mint;
        DrawCircleLines((int)warden.x, (int)warden.y, 110 + pulse * 5, Fade(color, 0.55f + pulse * 0.4f));
        DrawCircleLines((int)warden.x, (int)warden.y, 102, Fade(color, 0.3f + pulse * 0.3f));
    }
    if (feedback) DrawBattleEffects(duel, animationTime);
    DrawRectangleLinesEx(bounds, 2, CQ_BORDER);
    if (duel->phase == DUEL_INTRO) return;

    float bossHp = (float)duel->bossHp;
    float playerHp = (float)duel->playerHp;
    if (feedback) {
        bossHp += duel->damage * (1 - ProgressBetween(animationTime, bossHit, bossHit + 0.25f));
        playerHp += duel->incoming * (1 - ProgressBetween(animationTime, foxHit, foxHit + 0.25f));
    }
    DrawHealth(60, 120, "Null Warden", bossHp, WARDEN_HP, danger);
    DrawHealth(920, 365, "Bytefox", playerHp, BYTEFOX_HP, mint);
    DrawPanel((Rectangle){920, 439, 300, 45});
    Label(TextFormat("Mana %d / %d", duel->mana, BYTEFOX_MANA), 934, 451, 17, manaColor);
    for (int i = 0; i < BYTEFOX_MANA; i++) {
        Color fill = i < duel->mana ? manaColor : CQ_BORDER;
        DrawRectangle(1054 + i * 29, 457, 23, 8, fill);
    }

    if (duel->phase == DUEL_CHOOSE || duel->phase == DUEL_QUESTION) {
        int damage = DuelAttack(duel);
        const char *attack = damage == 8 ? "Buffer Crash" : damage == 4 ? "Loop Surge" : "Static Pulse";
        DrawPanel((Rectangle){60, 199, 300, 72});
        Label(TextFormat("Next: %s (%d HP)", attack, damage), 74, 208, 18, danger);
        Label(duel->shield ? "Shield active" : duel->exposed ? "Exposed: Strike boosted" : "Shield down",
              74, 237, 17, CQ_PALE);
    }
}

static bool DuelButton(int x, const char *label, bool primary)
{
    return Button((Rectangle){(float)x, 595, 366, 44}, label, primary);
}

void DrawDebugDuel(DuelState *duel)
{
    const int keys[] = {KEY_ONE, KEY_TWO, KEY_THREE};
    if (duel->phase == DUEL_FEEDBACK && animationTime < animationLength) {
        animationTime += GetFrameTime();
    }
    DrawArena(duel);
    DrawPanel((Rectangle){40, 502, 1200, 144});

    switch (duel->phase) {
        case DUEL_INTRO:
            Label("Debug Duel", 62, 511, 28, mint);
            Label("Read the code to power Bytefox's moves and defeat the Null Warden.", 62, 553, 20, CQ_TEXT);
            if (DuelButton(60, "Start duel", true) || IsKeyPressed(KEY_ENTER)) StartDuel(duel);
            break;
        case DUEL_CHOOSE:
            Label(TextFormat("Turn %d  |  Choose a move", duel->turn + 1), 62, 512, 24, mint);
            Label("Guard restores 2 mana on a correct answer. Break Shield costs 3 mana.",
                  62, 554, 19, CQ_TEXT);
            for (int i = 0; i < DUEL_MOVE_COUNT; i++) {
                int x = 60 + i * 295;
                Rectangle bounds = {(float)x, 595, 278, 44};
                if (i == DUEL_BREAK && (duel->exposed || duel->mana < BREAK_MANA_COST)) {
                    DrawRectangleRounded(bounds, 0.18f, 10, CQ_CARD);
                    Label(duel->exposed ? "Target already exposed" : "Need 3 mana", x + 18, 604, 19, CQ_MUTED);
                    continue;
                }
                if (Button(bounds, TextFormat("[%d]  %s", i + 1, moves[i]), false) ||
                    IsKeyPressed(keys[i])) {
                    ChooseDuelMove(duel, (DuelMove)i);
                    break;
                }
            }
            if (duel->phase == DUEL_CHOOSE) {
                Rectangle potion = {945, 595, 275, 44};
                bool available = duel->potions > 0 && duel->playerHp < BYTEFOX_HP && duel->potionHealing == 0;
                if (available) {
                    if (Button(potion, TextFormat("[4] Potion +%d HP (%d left)", POTION_HEAL, duel->potions), true)
                        || IsKeyPressed(KEY_FOUR)) DrinkDuelPotion(duel);
                } else {
                    DrawRectangleRounded(potion, 0.18f, 10, CQ_CARD);
                    const char *status = duel->potionHealing > 0 ? TextFormat("Healed +%d HP", duel->potionHealing)
                        : duel->potions == 0 ? "No potions left" : TextFormat("Full HP (%d potions)", duel->potions);
                    Label(status, 963, 604, 19, CQ_MUTED);
                }
            }
            break;
        case DUEL_QUESTION:
            Label(duel->question->code, 62, 508, 19, CQ_TEXT);
            Label(TextFormat("%s: %s", moves[duel->move], duel->question->prompt), 62, 563, 20, mint);
            for (int i = 0; i < 3; i++) {
                if (DuelButton(60 + i * 397, TextFormat("[%d]  %s", i + 1, duel->question->answers[i]), false)
                    || IsKeyPressed(keys[i])) {
                    shieldBeforeAnswer = duel->shield;
                    animationTime = 0;
                    AnswerDuel(duel, i);
                    break;
                }
            }
            break;
        case DUEL_FEEDBACK:
            if (animationTime < animationLength) {
                Label("Resolving turn...", 62, 514, 24, mint);
                break;
            }
            Label(TextFormat("%s  |  Warden -%d HP  |  Bytefox -%d HP",
                             duel->correct ? "Correct" : "Missed", duel->damage, duel->incoming),
                  62, 509, 23, duel->correct ? mint : danger);
            Label(duel->question->explanation, 62, 542, 20, CQ_TEXT);
            if (duel->phaseChanged)
                Label("Phase 2: the shield collapses. The Warden switches to loop challenges.",
                      62, 571, 18, CQ_PALE);
            if (DuelButton(60, "Continue", true) || IsKeyPressed(KEY_ENTER)) AdvanceDuel(duel);
            break;
        case DUEL_WON:
        case DUEL_LOST:
            Label(duel->phase == DUEL_WON ? "Victory! The Null Warden is defeated." : "Bytefox needs a rematch.",
                  62, 513, 26, duel->phase == DUEL_WON ? mint : danger);
            Label(TextFormat("Turns: %d   |   Bytefox HP: %d / %d", duel->turn + 1, duel->playerHp, BYTEFOX_HP),
                  62, 554, 20, CQ_TEXT);
            if (DuelButton(60, "Rematch", true) || IsKeyPressed(KEY_ENTER)) StartDuel(duel);
            break;
    }
}
