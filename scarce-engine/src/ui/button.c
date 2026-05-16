#include <assert.h>
#include <string.h>
#include "button.h"

#include "fixed_array.h"
#include "scarce.h"
#include "ui.h"
#include "memory/memory.h"
#include "physics/aabb.h"

void ui_button_init(ui_button* button, ui_button_callback callback, text_color* color, text_color* hoveredColor, u8 width)
{
    assert(button);

    button->width = width;
    button->callback = callback;
    button->color = *color;
    button->hoveredColor = *hoveredColor;
}

void ui_button_render(ui_button* button, ui_state* state, const char* content)
{
    assert(button);
    assert(state);
    assert(content);

    u32 contentLength = strlen(content);
    ui_set_color(state, button->isHovered ? &button->hoveredColor : &button->color);
    {
        fixed_array buffer = { 0 };
        fixed_array_init(&buffer, button->width);

        if (button->alignMiddle)
        {
            u32 spaceCount = (button->width - contentLength) / 2;

            for (u32 i = 0; i < spaceCount; ++i)
            {
                char space = ' ';
                fixed_array_push(&buffer, &space, 1); 
            }

            for (u32 i = 0; i < contentLength; ++i)
            {
                char current = content[i];
                fixed_array_push(&buffer, &current, 1); 
            }

            for (u32 i = 0; i < spaceCount; ++i)
            {
                char space = ' ';
                fixed_array_push(&buffer, &space, 1); 
            }
        }
        else if (button->alignRight)
        {

        }
        else
        {
            for (u32 i = 0; i < contentLength; ++i)
            {
                char current = content[i];
                fixed_array_push(&buffer, &current, 1); 
            }
            for (u32 i = contentLength; i < button->width; ++i)
            {
                char space = ' ';
                fixed_array_push(&buffer, &space, 1); 
            }
        }

        ui_text(state, buffer.buffer, buffer.current);
        fixed_array_destroy(&buffer);
    }


    
    button->x = state->container->prevX;
    button->y = state->container->prevY;
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