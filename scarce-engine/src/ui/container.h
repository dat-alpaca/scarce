#pragma once
#include "core/defines.h"
#include "batch_renderer.h"

struct ui_state;

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

struct container
{
    u16 prevX;
    u16 prevY;

    u16 _x;
    u16 _y;
    u16 height;
    u16 width;

    u16 currentX;
    i16 currentY;
    
    u8 positionOffset;
    u8 alignOffset;
    
    text_position position;
    text_align align;

    u8 sameline : 1;
    u8 overflow : 1;
    u8 xOverflow : 1;
    u8 firstLetter : 1;
} typedef container;

struct
{
    u32 length;
    bool noSpace;
} typedef container_overflow;

void container_init_default(container* container, batch_renderer* renderer);
void container_reset(container* container);
void container_fix_offset_bounds(container* container, u32 gridWidth, u32 gridHeight);

bool container_handle_y_overflow(container* container);
void container_handle_x_overflow(container* container);

u16* container_determine_x_from_align(container* container, u32 length, u32 gridWidth);
void container_set_align(container* container, text_align align, u16 offset);

i16* container_determine_y_from_position(container* container, u32 gridHeight);
void container_set_position(container* container, text_position position, u16 yOffset);

void container_feed(container* container);
void container_nudge(container* container, u32 amount);
void container_space(container* container, u32 amount);

void container_text(struct ui_state* state, container* container, const char* content, u32 length);