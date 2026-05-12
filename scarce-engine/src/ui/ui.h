#pragma once
#include "core/defines.h"
#include "core/memory/memory.h"
#include "physics/aabb.h"
#include "text_renderer.h"

#define SCA_UI_MAX_NUMBER_LENGTH 10 // u32

struct engine;

// Symbol:
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
} typedef symbol_color;

// Text rendering:
typedef enum : u8
{
    UI_ALIGN_LEFT = 0,
    UI_ALIGN_CENTER,
    UI_ALIGN_RIGHT,
} text_align;

typedef enum : u8
{
    UI_POS_NONE = 0,
    UI_POS_TOP,
    UI_POS_BOTTOM
} text_position;

typedef struct
{
    symbol_color color : 3;
    symbol_color background : 3;

    u8 colorIntense : 1;
    u8 colorFaint : 1;
    u8 backgroundIntense : 1;
    u8 backgroundFaint : 1;
    u8 renderBackground : 1;
} text_color;

typedef struct
{
    memory_pool* pool;
    text_renderer* renderer;

    u16 x;
    u16 y;
    u16 prevX;
    u16 prevY;
    u16 alignOffset;
    
    text_align alignment;
    text_position positioning;

    text_color color;
    bool sameLine; 
} ui_state;

void ui_begin(ui_state* state, memory_pool* pool, text_renderer* renderer);
ui_state* ui_begin_stack(memory_pool* pool, text_renderer* renderer);
void ui_end(ui_state* state);
void ui_clear(struct engine* e);

void ui_text(ui_state* state, const char* content, u32 length);
void ui_number(ui_state* state, u32 number);
void ui_text_absolute(ui_state* state, u32 x, u32 y, const char* content, u32 length);

void ui_set_align(ui_state* state, text_align align, u16 xOffset);
void ui_set_position(ui_state* state, text_position position, u16 yOffset);
void ui_set_color(ui_state* state, text_color* color);

void ui_sameline(ui_state* state, bool sameLine);
void ui_feed(ui_state* state);
void ui_nudge(ui_state* state, u32 xOffset);
void ui_space(ui_state* state, u32 yOffset);
void ui_hline(ui_state* state, u32 y, char lineChar);

aabb ui_mouse_aabb(struct engine* e);