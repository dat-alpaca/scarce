#pragma once
#include "ui/container.h"
#include "ui/ui.h"

struct engine;

typedef struct ui_scrollbar
{
    container* container;

    text_color color;
    text_color hoveredColor;
    
    u16 yOffset;
    
    char symbol;
    u8 isRendered : 1;
    u8 alwaysShow : 1;
} ui_scrollbar;

void ui_scrollbar_init(ui_scrollbar* scrollbar, container* container, text_color* color, text_color* hoveredColor, char symbol);
void ui_scrollbar_render(ui_scrollbar* scrollbar, struct ui_state* state);
void ui_scrollbar_update(ui_scrollbar* scrollbar, struct ui_state* state, struct engine* e);