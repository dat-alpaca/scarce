#include <engine.h>

#include "text_box.h"
#include "platform/key.h"
#include "platform/platform.h"
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
    textBox->current = 0;
    textBox->isKeyPressed = false;
    textBox->isRendered = false;

    for(u32 i = 0; i < width; ++i)
        textBox->contents[i] = '_';
}

void ui_text_box_render(ui_text_box* textBox, ui_state* state)
{
    assert(textBox);
    assert(state);
    textBox->isRendered = true;
    
    // Color:
    if (textBox->isHovered || textBox->isSelected)
        ui_set_color(state, &textBox->hoveredColor);
    else
        ui_set_color(state, &textBox->color);
    
    // Render:
    ui_sameline(state, true);
    ui_text(state, textBox->contents, textBox->width);

    // TODO:

    //textBox->x = state->prevX;
    //textBox->y = state->prevY;

    ui_sameline(state, false);
}

void ui_text_box_update(ui_text_box* textBox, ui_state* state, struct engine* e)
{
    assert(textBox);
    assert(state);

    if (!textBox->isRendered)
        return;

    aabb mouseAABB = ui_mouse_aabb(e);
    {
        aabb textBoxAABB = ui_text_box_aabb(textBox);
        textBox->isHovered = false;
        
        if (aabb_check_collision(mouseAABB, textBoxAABB))
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
    }

    bool anyKeyDown = false;
    if (window_is_key_pressed(e->window, SCA_KEY_BACKSPACE))
        anyKeyDown = true;

    if (window_is_key_pressed(e->window, SCA_KEY_SPACE))
        anyKeyDown = true;

    for (u32 i = 0; i < 10; ++i)
        if (window_is_key_pressed(e->window, SCA_KEY_0 + i)) 
            anyKeyDown = true;
    
    for (u32 i = 0; i < 26; ++i)
        if (window_is_key_pressed(e->window, SCA_KEY_A + i)) 
            anyKeyDown = true;

    if (!anyKeyDown)
        textBox->isKeyPressed = false;

    if (textBox->isSelected && !textBox->isKeyPressed)
    {
        if (window_is_key_pressed(e->window, SCA_KEY_ENTER))
            textBox->isSelected = false;

        if (window_is_key_pressed(e->window, SCA_KEY_SPACE))
        {
            if (textBox->current < textBox->width)
            {
                textBox->isKeyPressed = true;
                textBox->contents[textBox->current] = ' ';
                ++textBox->current;
            }
        }
        else if (window_is_key_pressed(e->window, SCA_KEY_BACKSPACE))
        {
            if(textBox->current >= 0)
            {
                textBox->isKeyPressed = true;
                
                if (textBox->current != 0)
                    --textBox->current;
                textBox->contents[textBox->current] = '_';
            }
        }
        else
        {
            for(u32 i = 0; i < 10; ++i)
            {
                if (!window_is_key_pressed(e->window, SCA_KEY_0 + i) || textBox->isKeyPressed)
                    continue;

                if (textBox->current < textBox->width)
                {
                    textBox->isKeyPressed = true;
                    textBox->contents[textBox->current] = '0' + i;
                    ++textBox->current;
                }
            }

            for(u32 i = 0; i < 26; ++i)
            {
                if (!window_is_key_pressed(e->window, SCA_KEY_A + i) || textBox->isKeyPressed)
                    continue;

                if (textBox->current < textBox->width)
                {
                    textBox->isKeyPressed = true;
                    textBox->contents[textBox->current] = 'a' + i;
                    ++textBox->current;
                }
            }
        }
    }

    textBox->isRendered = false;
}

aabb ui_text_box_aabb(ui_text_box* textBox)
{
    aabb textBoxAABB = { 0 };
    {
        textBoxAABB.x = textBox->x;
        textBoxAABB.y = textBox->y;
        textBoxAABB.width = textBox->width;
        textBoxAABB.height = 1;
    }

    return textBoxAABB;
}