#include "container.h"

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
    if (container->width > gridWidth)
        container->width = gridWidth;

    if (container->height > gridHeight)
        container->height = gridHeight;

    if (container->currentX > gridWidth - container->width)
        container->currentX = gridWidth - container->width;

    if (container->currentY > gridHeight - container->height)
        container->currentY = gridHeight - container->height;
}

bool container_handle_y_overflow(container* container)
{
    if (container->currentY < 0 || container->currentY >= container->_y + container->height)
    {
        container->currentX = container->_x + container->alignOffset; // TODO: check center, right
        container->overflow = true;
        return true;
    }

    return false;
}
void container_handle_x_overflow(container* container, const char* content)
{
    if (container->currentX < container->_x + container->width)
        return;

    container_space(container, 1);
    
    container->currentX = container->_x + container->alignOffset;
    container->xOverflow = true;
}

void container_determine_x_from_align(container* container, u32 length, u32 gridWidth)
{   
    container->xOverflow = false;

    switch (container->align)
    {
        case UI_ALIGN_CENTER:
        {
            if (container->_x + container->width >= gridWidth)
                container->_x = gridWidth - container->width;
            
            if (length >= container->width)
            {
                container->currentX = container->_x + container->alignOffset;
                return;
            }

            u32 usedWidth = (container->width % 2 == 0) ? container->width / 2 : container->width / 2 + 1;
            container->currentX = container->_x + container->alignOffset + usedWidth - length / 2;
        } break;
        
        case UI_ALIGN_LEFT:
        {
            if (container->_x + container->width >= gridWidth)
                container->_x = gridWidth - container->width;

            container->currentX = container->_x + container->alignOffset;
        } break;

        case UI_ALIGN_RIGHT:
        {
            if (container->_x + container->width >= gridWidth)
                container->_x = gridWidth - container->width;

            if (length >= container->width)
            {
                container->currentX = container->_x + container->alignOffset;
                return;
            }

            container->currentX = container->width - length - container->alignOffset + container->_x + 1;
        } break;
    }
}
void container_set_align(container* container, text_align align, u16 offset)
{
    container->align = align;
    container->alignOffset = (offset < container->width) ? offset : container->width - 1;
}

void container_determine_y_from_position(container* container, u32 gridHeight)
{
    if (container->_y + container->height >= gridHeight)
        container->_y = gridHeight - container->height;

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
    container->currentX = container->_x + container->alignOffset;
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