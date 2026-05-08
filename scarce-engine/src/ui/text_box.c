#include "text_box.h"
#include "ui/ui.h"
#include "core/physics/aabb.h"

void ui_text_box_init(ui_text_box* textBox, char* contents, text_color* color, text_color* hoveredColor, u8 width)
{
    assert(textBox);
    assert(contents);

    textBox->contents = contents;
    textBox->color = *color;
    textBox->hoveredColor = *hoveredColor;
    textBox->width = width;

    for(u32 i = 0; i < width; ++i)
        textBox->contents[i] = '_';
}

void ui_text_box_render(ui_text_box* textBox, ui_state* state)
{
    assert(textBox);
    assert(state);
    
    // Color:
    if (textBox->isHovered || textBox->isSelected)
        ui_set_color(state, &textBox->hoveredColor);
    else
        ui_set_color(state, &textBox->color);
    
    // Render:
    ui_sameline(state, true);
    ui_text(state, textBox->contents, textBox->width);

    textBox->x = state->prevX;
    textBox->y = state->prevY;

    ui_sameline(state, false);
}

void ui_text_box_update(ui_text_box* textBox, ui_state* state, struct engine* e)
{
    assert(textBox);
    assert(state);

    memory_pool* pool = state->pool;

    aabb* mouseAABB = scarce_push(pool, sizeof(aabb));
    {
        u32* mouseX = scarce_push(pool, sizeof(u32));
        u32* mouseY = scarce_push(pool, sizeof(u32));

        text_renderer_get_mouse_grid_position(e->window, e->renderer, mouseX, mouseY);
        mouseAABB->x = *mouseX;
        mouseAABB->y = *mouseY;
        mouseAABB->width = 1;
        mouseAABB->height = 1;

        scarce_pop(pool, sizeof(u32));
        scarce_pop(pool, sizeof(u32));
    }

    {
        aabb* textBoxAABB = scarce_push(pool, sizeof(aabb));
        textBoxAABB->x = textBox->x;
        textBoxAABB->y = textBox->y;
        textBoxAABB->width = textBox->width;
        textBoxAABB->height = 1;

        textBox->isHovered = false;
        
        if (aabb_check_collision(*mouseAABB, *textBoxAABB))
        {
            textBox->isHovered = true;

            if (window_is_mouse_btn_pressed(e->window, SCA_MOUSE_LEFT))
                textBox->isSelected = true;
        }
        else
        {
            // Deselect when pressed outside:
            if (window_is_mouse_btn_pressed(e->window, SCA_MOUSE_LEFT))
                textBox->isSelected = false;
        }

        scarce_pop(pool, sizeof(aabb));
    }

    if (textBox->isSelected)
    {

    }

    scarce_pop(pool, sizeof(aabb));
}