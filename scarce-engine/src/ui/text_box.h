#pragma once
#include "physics/aabb.h"
#include "ui/ui.h"

struct engine;

typedef struct ui_text_box
{
    char* contents;
    text_color color;
    text_color hoveredColor;

    u8 width;
    u8 x;
    u8 y;

    u8 isHovered : 1;
    u8 isSelected : 1;
    u8 isKeyPressed : 1;
    u8 isRendered : 1;
    
    u8 current : 4;
} ui_text_box;

void ui_text_box_init(ui_text_box* textBox, char* contents, text_color* color, text_color* hoveredColor, u8 width);
void ui_text_box_render(ui_text_box* textBox, ui_state* state);
void ui_text_box_update(ui_text_box* textBox, ui_state* state, struct engine* e);

aabb ui_text_box_aabb(ui_text_box* textBox);