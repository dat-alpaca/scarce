#pragma once
#include <stdbool.h>
#include <core/defines.h>

typedef struct
{
    u16 x;
    u16 y;
    u16 width;
    u16 height;
} aabb;

bool aabb_check_collision(aabb a, aabb b);