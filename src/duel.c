#include "duel.h"

static const DuelQuestion questions[2][DUEL_MOVE_COUNT][2] = {
    {
        {
            {"int x = 2 + 3 * 4;", "What is x?", {"20", "14", "24"},
             "Multiply 3 by 4 first, then add 2.", 1},
            {"int x = 17 % 5;", "What is x?", {"2", "3", "5"},
             "17 divided by 5 leaves a remainder of 2.", 0}
        },
        {
            {"int a[3] = {2, 4, 6};", "Which read goes out of bounds?", {"a[0]", "a[2]", "a[3]"},
             "The valid indexes are 0, 1 and 2.", 2},
            {"int divisor = 0;", "Which expression must be avoided?", {"12 / divisor", "12 + divisor", "12 * divisor"},
             "Integer division by zero has undefined behavior.", 0}
        },
        {
            {"int a[] = {2, 4, 6};\na[1] += 3;", "What is a[1] now?", {"5", "7", "9"},
             "Index 1 is the second cell. Add 3 to its value, 4.", 1},
            {"int a[] = {5, 8, 3};\na[0] = a[2] + 1;", "What is a[0] now?", {"6", "9", "4"},
             "Read a[2], which is 3, then write 3 + 1 to a[0].", 2}
        }
    },
    {
        {
            {"int n = 0;\nfor (int i = 0; i < 3; i++) n += 2;",
             "What is n after the loop?", {"6", "4", "8"},
             "The loop adds 2 for each i from 0 through 2.", 0},
            {"int n = 1;\nfor (int i = 0; i < 3; i++) n *= 2;",
             "What is n after the loop?", {"6", "8", "16"},
             "Three doublings take n from 1 to 2 to 4 to 8.", 1}
        },
        {
            {"int a[3] = {2, 4, 6};\nfor (int i = 0; i <= 3; i++) use(a[i]);",
             "Which condition fixes the loop?", {"i <= 3", "i < 3", "i < 4"},
             "Stop before index 3. The last valid index is 2.", 1},
            {"int i = 0;\nwhile (i < 3) { total += i; }",
             "What makes this loop terminate?", {"Reset i to 0", "Increment total", "Increment i"},
             "Increment i so the condition eventually becomes false.", 2}
        },
        {
            {"int n = 0;\nfor (int i = 1; i <= 3; i++) n += i;",
             "What is n after the loop?", {"3", "6", "9"},
             "The loop adds 1, 2 and 3, for a total of 6.", 1},
            {"int n = 0;\nfor (int i = 0; i < 5; i += 2) n++;",
             "What is n after the loop?", {"2", "5", "3"},
             "The body runs for i equal to 0, 2 and 4.", 2}
        }
    }
};

void StartDuel(DuelState *duel)
{
    *duel = (DuelState){.phase = DUEL_CHOOSE, .playerHp = BYTEFOX_HP,
                       .bossHp = WARDEN_HP, .mana = BREAK_MANA_COST,
                       .potions = DUEL_POTIONS, .shield = true};
}

bool DrinkDuelPotion(DuelState *duel)
{
    if (duel->phase != DUEL_CHOOSE || duel->potions == 0 ||
        duel->playerHp == BYTEFOX_HP || duel->potionHealing > 0) return false;

    int missing = BYTEFOX_HP - duel->playerHp;
    duel->potionHealing = missing < POTION_HEAL ? missing : POTION_HEAL;
    duel->playerHp += duel->potionHealing;
    duel->potions--;
    return true;
}

int DuelAttack(const DuelState *duel)
{
    if (duel->turn % 3 == 1) return 8;
    return duel->bossHp <= WARDEN_PHASE_TWO_HP ? 4 : 3;
}

bool ChooseDuelMove(DuelState *duel, DuelMove move)
{
    if (duel->phase != DUEL_CHOOSE || move < DUEL_STRIKE || move >= DUEL_MOVE_COUNT)
        return false;
    if (move == DUEL_BREAK) {
        if (duel->exposed || duel->mana < BREAK_MANA_COST) return false;
        duel->mana -= BREAK_MANA_COST;
    }

    int stage = duel->bossHp <= WARDEN_PHASE_TWO_HP;
    int variant = duel->uses[stage][move]++ % 2;
    duel->move = move;
    duel->question = &questions[stage][move][variant];
    duel->phase = DUEL_QUESTION;
    return true;
}

bool AnswerDuel(DuelState *duel, int answer)
{
    if (duel->phase != DUEL_QUESTION || answer < 0 || answer >= 3) return false;

    bool wasPhaseTwo = duel->bossHp <= WARDEN_PHASE_TWO_HP;
    duel->selected = answer;
    duel->correct = answer == duel->question->correct;
    duel->damage = 0;
    duel->incoming = DuelAttack(duel);
    if (duel->correct) {
        switch (duel->move) {
            case DUEL_STRIKE:
                duel->damage = duel->exposed ? 9 : duel->shield ? 2 : 5;
                duel->exposed = false;
                break;
            case DUEL_GUARD:
                duel->incoming = 0;
                duel->damage = 2;
                duel->mana += 2;
                if (duel->mana > BYTEFOX_MANA) duel->mana = BYTEFOX_MANA;
                break;
            case DUEL_BREAK:
                duel->shield = false;
                duel->exposed = true;
                duel->damage = 1;
                break;
            case DUEL_MOVE_COUNT:
                break;
        }
    }
    if (duel->damage >= duel->bossHp) {
        duel->damage = duel->bossHp;
        duel->incoming = 0;
    }
    if (duel->incoming > duel->playerHp) duel->incoming = duel->playerHp;
    duel->bossHp -= duel->damage;
    duel->playerHp -= duel->incoming;
    duel->phaseChanged = !wasPhaseTwo && duel->bossHp > 0 && duel->bossHp <= WARDEN_PHASE_TWO_HP;
    if (duel->phaseChanged) duel->shield = false;
    duel->phase = DUEL_FEEDBACK;
    return true;
}

void AdvanceDuel(DuelState *duel)
{
    if (duel->phase != DUEL_FEEDBACK) return;
    if (duel->bossHp == 0) duel->phase = DUEL_WON;
    else if (duel->playerHp == 0) duel->phase = DUEL_LOST;
    else {
        duel->turn++;
        duel->potionHealing = 0;
        duel->phase = DUEL_CHOOSE;
    }
}
