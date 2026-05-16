#pragma once
#include "ui/ui.h"

struct engine;

typedef struct ui_button
{
    void (*callback)(struct engine* e, memory_pool* pool, struct ui_button* button);
    text_color color;
    text_color hoveredColor;
    
    u8 width;
    u8 x, y;

    u8 isHovered  : 1;
    u8 isPressed  : 1;
    u8 isRendered : 1;
} ui_button;

typedef void (*ui_button_callback)(struct engine* e, memory_pool* pool, struct ui_button* button);

void ui_button_init(ui_button* button, ui_button_callback callback, text_color* color, text_color* hoveredColor, u8 width);
void ui_button_render(ui_button* button, ui_state* state, const char* content);
void ui_button_update(ui_button* button, ui_state* state, struct engine* e);

aabb ui_button_aabb(ui_button* button);