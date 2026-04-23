#pragma once
#include "scarce.h"

#include "core/defines.h"
#include "core/memory/memory.h"

enum
{
    SY_COLOR_NONE = 0,
    SY_COLOR_RED,
    SY_COLOR_GREEN,
    SY_COLOR_YELLOW,
    SY_COLOR_BLUE,
    SY_COLOR_PURPLE,
    SY_COLOR_CYAN,
    SY_COLOR_WHITE,
} typedef symbol_color;

struct
{
    u8 glyphID                          : 4;

    u8 isColorIntense                   : 1;
    u8 color                            : 3;
    
    u8 isBackgroundColorIntense         : 1;
    u8 backgroundColor                  : 3;

    u8 bold                             : 1;
    u8 strike                           : 1;
    u8 underline                        : 1;
    u8 faint                            : 1;
} typedef symbol;

void render_file(engine* e, memory_pool* pool, const char* filepath, u32 fileDescriptorAddr);
void render_vram(engine* e, memory_pool* pool, u32 vramAddress, u32 vramSizeBytes, u32 symbolTableAddress, u8 width, u8 height, u8 offset);