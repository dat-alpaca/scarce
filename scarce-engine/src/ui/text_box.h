#pragma once
#include "ui/ui.h"

typedef struct a
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
    
    u8 current : 5;
} ui_text_box;

void ui_text_box_init(ui_text_box* textBox, char* contents, text_color* color, text_color* hoveredColor, u8 width);
void ui_text_box_render(ui_text_box* textBox, ui_state* state);
void ui_text_box_update(ui_text_box* textBox, ui_state* state, struct engine* e);