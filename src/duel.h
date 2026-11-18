#ifndef CODEQUEST_DUEL_H
#define CODEQUEST_DUEL_H

#include <stdbool.h>

typedef enum {
    DUEL_INTRO, DUEL_CHOOSE, DUEL_QUESTION, DUEL_FEEDBACK, DUEL_WON, DUEL_LOST
} DuelPhase;

typedef enum { DUEL_STRIKE, DUEL_GUARD, DUEL_BREAK, DUEL_MOVE_COUNT } DuelMove;

typedef struct {
    const char *code;
    const char *prompt;
    const char *answers[3];
    const char *explanation;
    int correct;
} DuelQuestion;

enum {
    BYTEFOX_HP = 24, WARDEN_HP = 24, WARDEN_PHASE_TWO_HP = 12,
    BYTEFOX_MANA = 5, BREAK_MANA_COST = 3, DUEL_POTIONS = 2, POTION_HEAL = 7
};

typedef struct {
    DuelPhase phase;
    DuelMove move;
    int playerHp;
    int bossHp;
    int mana;
    int potions;
    int potionHealing;
    int turn;
    int uses[2][DUEL_MOVE_COUNT];
    int selected;
    int damage;
    int incoming;
    bool shield;
    bool exposed;
    bool correct;
    bool phaseChanged;
    const DuelQuestion *question;
} DuelState;

void StartDuel(DuelState *duel);
bool DrinkDuelPotion(DuelState *duel);
int DuelAttack(const DuelState *duel);
bool ChooseDuelMove(DuelState *duel, DuelMove move);
bool AnswerDuel(DuelState *duel, int answer);
void AdvanceDuel(DuelState *duel);
void DrawDebugDuel(DuelState *duel);
void LoadDuelArt(void);
void UnloadDuelArt(void);

#endif
