#include "container.h"

void container_init_default(container* container, text_renderer* renderer)
{
    container->prevX = 0;
    container->prevY = 0;
    container->x = 0;
    container->y = 0;

    container->position = UI_POS_NONE;
    container->positionOffset = 0;

    container->align = UI_ALIGN_LEFT;
    container->alignOffset = 0;

    container->sameline = 0;
    container->overflow = 0;

    container->width = text_renderer_width(renderer);
    container->height = text_renderer_height(renderer);
}

bool container_handle_y_overflow(container* container)
{
    if (container->y < 0)
    {
        container->x = 0;
        container->overflow = true;
        return true;
    }

    if (container->y >= container->height)
    {
        container->x = 0;
        container->overflow = true;
        return true;
    }

    return false;
}
void container_handle_x_overflow(container* container, const char* content)
{
    if (container->x < container->width)
        return;

    if (container->position != UI_POS_BOTTOM)
        ++container->y;
    else
        --container->y;

    switch (container->align)
    {
        case UI_ALIGN_RIGHT:
            container->x = container->prevX;
            break;

        case UI_ALIGN_CENTER:
        {
            container->x = container->width / 2;
            break;
        }

        case UI_ALIGN_LEFT:
            container->x = container->alignOffset;
            break;
    }
}

void container_determine_x_from_align(container* container, u32 length)
{
    switch (container->align)
    {
        case UI_ALIGN_CENTER:
            container->x += (container->width / 2 - length / 2) + container->alignOffset;
            break;
        
        case UI_ALIGN_LEFT:
            container->x += container->alignOffset;
            break;

        case UI_ALIGN_RIGHT:
            container->x += container->width - length - container->alignOffset;
            break;
    }
}
void container_set_align(container* container, text_align align, u16 offset)
{
    container->align = align;
    container->alignOffset = (offset < container->width) ? offset : container->width;
}

void container_determine_y_from_position(container* container)
{
    if (container->position == UI_POS_BOTTOM)
        container->y = container->height - container->positionOffset - 1;
}
void container_set_position(container* container, text_position position, u16 yOffset)
{
    container->position = position;
    container->positionOffset = yOffset;
}

void container_nudge(container* container, u32 amount)
{
    container->x += amount;
}
void container_space(container* container, u32 amount)
{
    switch (container->position)
    {
        case UI_POS_BOTTOM:
            container->y -= amount;
            break;

        case UI_POS_NONE:
        case UI_POS_TOP:
            container->y += amount;
            break;
    }
}