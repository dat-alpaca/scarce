#include <assert.h>
#include "button.h"

#include "scarce.h"
#include "ui.h"
#include "memory/memory.h"
#include "physics/aabb.h"

void ui_button_init(ui_button* button, ui_button_callback callback, text_color* color, text_color* hoveredColor, u8 width)
{
    assert(button);
    assert(width > 0);

    button->callback = callback;
    button->color = *color;
    button->hoveredColor = *hoveredColor;
    button->width = width;
}

void ui_button_render(ui_button* button, ui_state* state, const char* content)
{
    assert(button);
    assert(state);
    assert(content);
    
    ui_set_color(state, button->isHovered ? &button->hoveredColor : &button->color);
    ui_text(state, content, button->width);
    button->x = state->prevX;
    button->y = state->prevY;
    button->isRendered = true;
}

void ui_button_update(ui_button* button, ui_state* state, engine* e)
{
    assert(button);
    assert(state);

    if (!button->isRendered)
        return;

    memory_pool* pool = state->pool;

    aabb mouseAABB = ui_mouse_aabb(e);

    {
        aabb buttonAABB = ui_button_aabb(button);
        button->isHovered = false;
        
        if (aabb_check_collision(mouseAABB, buttonAABB))
        {
            button->isHovered = true;

            if (window_is_mouse_btn_pressed(e->window, SCA_MOUSE_LEFT) && button->callback)
            {
                if (!button->isPressed)
                    button->callback(e, pool, button);

                button->isPressed = true;
            }
            else
                button->isPressed = false;
        }
    }
    
    button->isRendered = false;
}

aabb ui_button_aabb(ui_button* button)
{
    aabb buttonAABB = { 0 };
    {
        buttonAABB.x = button->x;
        buttonAABB.y = button->y;
        buttonAABB.width = button->width;
        buttonAABB.height = 1;
    }

    return buttonAABB;
}