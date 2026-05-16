#pragma once
#include "core/defines.h"
#include "text_renderer.h"

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

struct
{
    u16 prevX;
    u16 prevY;

    u16 x;
    i32 y;
    u16 width;
    u16 height;
    
    text_position position;
    u16 positionOffset;

    text_align align;
    u16 alignOffset;

    u8 sameline : 1;
    u8 overflow : 1;
} typedef container;

struct
{
    u32 length;
    bool noSpace;
} typedef container_overflow;

void container_init_default(container* container, text_renderer* renderer);

bool container_handle_y_overflow(container* container);
void container_handle_x_overflow(container* container, const char* content);

void container_determine_x_from_align(container* container, u32 length);
void container_set_align(container* container, text_align align, u16 offset);

void container_determine_y_from_position(container* container);
void container_set_position(container* container, text_position position, u16 yOffset);

void container_nudge(container* container, u32 amount);
void container_space(container* container, u32 amount);