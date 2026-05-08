#include <assert.h>
#include "button.h"

#include "scarce.h"
#include "ui.h"
#include "memory/memory.h"
#include "physics/aabb.h"

void ui_button_init(ui_button* button, ui_button_callback callback, text_color* color, text_color* hoveredColor, u8 width)
{
    assert(button);
    assert(callback);
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
}

void ui_button_update(ui_button* button, ui_state* state, engine* e)
{
    assert(button);
    assert(state);

    memory_pool* pool = state->pool;

    aabb* mouseAABB = scarce_push(pool, sizeof(aabb));
    {
        u32* mouseX = scarce_push(pool, sizeof(u32));
        u32* mouseY = scarce_push(pool, sizeof(u32));

        e->get_mouse_position(e->window, e->renderer, mouseX, mouseY);
        mouseAABB->x = *mouseX;
        mouseAABB->y = *mouseY;
        mouseAABB->width = 1;
        mouseAABB->height = 1;

        e->scarce_pop(pool, sizeof(u32));
        e->scarce_pop(pool, sizeof(u32));
    }

    {
        aabb* buttonAABB = e->scarce_push(pool, sizeof(aabb));
        buttonAABB->x = state->lastX;
        buttonAABB->y = state->yOffset;
        buttonAABB->width = button->width;
        buttonAABB->height = 1;

        button->isHovered = false;
        
        if (aabb_check_collision(*mouseAABB, *buttonAABB))
        {
            button->isHovered = true;

            if (e->is_mouse_btn_pressed(e->window, SCA_MOUSE_LEFT) && button->callback)
            {
                if (!button->isPressed)
                    button->callback(e, pool, button);

                button->isPressed = true;
            }
            else
                button->isPressed = false;
        }
        
        e->scarce_pop(pool, sizeof(aabb));
    }

    e->scarce_pop(pool, sizeof(aabb));
}