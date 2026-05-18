#include "container.h"
#include "defines.h"
#include "color.h"
#include "ui.h"

#include <ctype.h>

void container_init_default(container* container, text_renderer* renderer)
{
    container_reset(container);

    container->width = text_renderer_width(renderer);
    container->height = text_renderer_height(renderer);
}
void container_reset(container* container)
{
    container->prevX = 0;
    container->prevY = 0;
    container->_x = 0;
    container->_y = 0;
    container->currentX = 0;
    container->currentY = 0;
    container->width = 0;
    container->height = 0;

    container->position = UI_POS_NONE;
    container->positionOffset = 0;

    container->align = UI_ALIGN_LEFT;
    container->alignOffset = 0;

    container->sameline = 0;
    container->overflow = 0;
}
void container_fix_offset_bounds(container* container, u32 gridWidth, u32 gridHeight)
{
    container->currentX = container->_x + container->alignOffset;
    container->currentY = container->_y + container->positionOffset;

    if (container->width > gridWidth)
        container->width = gridWidth;

    if (container->height > gridHeight)
        container->height = gridHeight;

    if (container->_x + container->width >= gridWidth)
        container->_x = gridWidth - container->width;

    if (container->_y + container->height >= gridHeight)
        container->_y = gridHeight - container->height;

    if (container->currentX > gridWidth - container->width)
        container->currentX = gridWidth - container->width;

    if (container->currentY > gridHeight - container->height)
        container->currentY = gridHeight - container->height;
}

bool container_handle_y_overflow(container* container)
{
    if (container->currentY < 0 || container->currentY >= container->_y + container->height)
    {
        container->currentX = container->_x + container->alignOffset;
        container->overflow = true;
        return true;
    }

    return false;
}
void container_handle_x_overflow(container* container)
{
    if (container->currentX < container->_x + container->width)
        return;

    container_space(container, 1);
    container_feed(container);

    container->xOverflow = true;
}

u16* container_determine_x_from_align(container* container, u32 length, u32 gridWidth)
{   
    container->xOverflow = false;
    
    switch (container->align)
    {
        case UI_ALIGN_RIGHT:
        {
            if (!container->firstLetter)
                break;

            if (length < container->width)
                break;

            container->currentX = container->_x;
        } break;

        case UI_ALIGN_CENTER:
        {
            if (!container->firstLetter)
                break;

            if (length >= container->width)
                break;

            u32 usedWidth = (container->width % 2 == 0) ? (container->width / 2) : container->width / 2 + 1;
            container->currentX = container->_x + container->alignOffset + usedWidth - (length / 2);
        } break;

        default:
            break;
    }

    container->firstLetter = false;
    return &container->currentX;
}
void container_set_align(container* container, text_align align, u16 offset)
{
    container->align = align;
    container->alignOffset = (offset < container->width) ? offset : container->width - 1;
    
    container_feed(container);
}

void container_determine_y_from_position(container* container, u32 gridHeight)
{
    if (container->position == UI_POS_BOTTOM)
        container->currentY = container->height - container->positionOffset - 1;
}
void container_set_position(container* container, text_position position, u16 yOffset)
{
    container->position = position;
    container->positionOffset = yOffset;
}

void container_feed(container* container)
{
    container->firstLetter = true;

    switch (container->align)
    {
        case UI_ALIGN_LEFT:
        case UI_ALIGN_CENTER:
            container->currentX = container->_x + container->alignOffset;
            break;
        
        case UI_ALIGN_RIGHT:
            container->currentX = container->_x + container->width - container->alignOffset - 1;
            break;
    }
}
void container_nudge(container* container, u32 amount)
{
    container->currentX += amount;
}
void container_space(container* container, u32 amount)
{
    // TODO: clamp boundaries 

    switch (container->position)
    {
        case UI_POS_BOTTOM:
        {
            container->currentY -= amount;
            break;
        } 

        case UI_POS_NONE:
        case UI_POS_TOP:
            container->currentY += amount;
            break;
    }
}

static i32 container_determine_starting_index(container* container, u32 length)
{
    return (container->align != UI_ALIGN_RIGHT) ? 0 : length - 1;
}

static bool container_should_stop_text(container* container, i32 i, u32 length)
{
    if (container->align != UI_ALIGN_RIGHT)
        return (i >= length);
    else
        return (i < 0);
}

static void container_step(container* container, i32* i)
{
    switch (container->align)
    {
        case UI_ALIGN_LEFT:
        case UI_ALIGN_CENTER:
        {
            ++container->currentX;
            ++(*i);
        } break;
            
        case UI_ALIGN_RIGHT:
        {
            --container->currentX;
            --(*i);
        } break;
    }
}

void container_text(ui_state* state, container* container, const char* content, u32 length)
{
    float* color = get_color_with_flags(state->color.color, state->color.colorIntense, state->color.colorFaint);
    float* background = get_color_with_flags(state->color.background, state->color.backgroundIntense, state->color.backgroundFaint);

    container->prevX = container->currentX;
    container->prevY = container->currentY;

    for (u32 x = container->_x; x < container->_x + container->width; ++x)
    {
        for (u32 y = container->_y; y < container->_y + container->height; ++y)
            text_renderer_set_character_background_color(state->renderer, x, y, background[0], background[1], background[2], state->color.renderBackground);
    }
    
    i32 i = container_determine_starting_index(container, length - 1);
    while (true)
    {
        if (container_should_stop_text(container, i, length))
            break;

        if (content[i] == '\0')
            break;

        u16* x = container_determine_x_from_align(container, length, text_renderer_width(state->renderer)); 
        i16* y = &container->currentY; 

        container_handle_x_overflow(container);
        if (container->xOverflow)
        {
            while (isspace(content[i])) i++;
            container->xOverflow = false;
        }

        if (container_handle_y_overflow(container))
            break;

        text_renderer_set_character_letter(state->renderer, *x, *y, content[i]);
        text_renderer_set_character_color(state->renderer, *x, *y, color[0], color[1], color[2]);
        text_renderer_set_character_background_color(state->renderer, *x, *y, background[0], background[1], background[2], state->color.renderBackground);
    
        container_step(container, &i);        
    }

    if (container->sameline)
        return;

    ui_feed(state);
    ui_space(state, 1);
}