#include "anim.h"
#include <math.h>
#include <string.h>

typedef struct {
    uint32_t id;
    float value;
    double lastUsed;
    bool occupied;
} Animation;

static Animation animations[256];

uint32_t AnimIdRect(Rectangle bounds)
{
    float coordinates[] = {bounds.x, bounds.y, bounds.width, bounds.height};
    uint32_t hash = 2166136261u;
    for (int i = 0; i < 4; i++) {
        uint32_t bits;
        memcpy(&bits, &coordinates[i], sizeof bits);
        hash = (hash ^ bits) * 16777619u;
    }
    return hash;
}

float AnimValue(uint32_t id, float target, float speed)
{
    double now = GetTime();
    Animation *slot = NULL;
    Animation *oldest = &animations[0];
    for (int i = 0; i < 256; i++) {
        Animation *entry = &animations[i];
        if (entry->occupied && entry->id == id) {
            slot = entry;
            break;
        }
        if (!entry->occupied && !slot) slot = entry;
        if (entry->lastUsed < oldest->lastUsed) oldest = entry;
    }
    if (!slot) slot = oldest;
    if (!slot->occupied || slot->id != id || now - slot->lastUsed > 0.25)
        *slot = (Animation){.id = id, .occupied = true};
    slot->lastUsed = now;
    float dt = GetFrameTime();
    if (dt > 0.1f) dt = 0.1f;
    if (dt < 0) dt = 0;
    if (speed <= 0) slot->value = target;
    else slot->value += (target - slot->value) * (1 - expf(-speed * dt));
    return slot->value;
}
