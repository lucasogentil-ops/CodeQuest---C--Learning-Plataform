#ifndef CODEQUEST_ANIM_H
#define CODEQUEST_ANIM_H
#include "raylib.h"
#include <stdint.h>
uint32_t AnimIdRect(Rectangle bounds);
float AnimValue(uint32_t id, float target, float speed);
#endif
