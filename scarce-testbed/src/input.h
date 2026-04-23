#pragma once
#include "core/defines.h"

typedef struct 
{
    u8 up           : 1;
    u8 down         : 1;
    u8 left         : 1;
    u8 right        : 1;
    u8 reserved     : 4;
} player_input;