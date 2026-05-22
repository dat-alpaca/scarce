#pragma once
#include "core/defines.h"

enum : u8
{
    SY_COLOR_NONE = 0,
    SY_COLOR_RED,
    SY_COLOR_GREEN,
    SY_COLOR_YELLOW,
    SY_COLOR_BLUE,
    SY_COLOR_PURPLE,
    SY_COLOR_CYAN,
    SY_COLOR_WHITE,
} typedef color;

float* get_color_with_flags(u32 symbolColor, bool isIntense, bool isFaint);