#include "ui.h"
#include "core/memory/memory.h"
#include <stdbool.h>
#include "engine.h"
#include "memory/stack.h"
#include "physics/aabb.h"
#include "text_renderer.h"

#include "ui/color.h"
#include "ui/container.h"

static void ui_reset(ui_state* state)
{
    container_init_default(&state->container, state->renderer);
    state->defaultContainer = true;
    
    state->color.color = SY_COLOR_NONE;
    state->color.colorIntense = false;
    state->color.colorFaint = false;

    state->color.background = SY_COLOR_NONE;
    state->color.backgroundIntense = false;
    state->color.backgroundFaint = false;

    state->color.renderBackground = false;
}

void ui_begin(ui_state* state, memory_pool* pool, text_renderer* renderer)
{
    assert(state);
    assert(pool);
    assert(renderer);

    state->pool = pool;
    state->renderer = renderer;

    ui_reset(state);
}
ui_state* ui_begin_stack(memory_pool* pool, text_renderer* renderer)
{
    assert(pool);
    assert(renderer);

    ui_state* state = scarce_push(pool, sizeof(ui_state));
    state->pool = pool;
    state->renderer = renderer;
    state->stackState = true;

    ui_reset(state);
    return state;
}
void ui_end(ui_state* state)
{
    assert(state);

    if (state->stackState)
        scarce_pop(state->pool, sizeof(ui_state));
}
void ui_clear(engine* e)
{
    assert(e);
    text_renderer_zero_buffer(e->renderer);
}

void ui_text(ui_state* state, const char* content, u32 length)
{
    assert(state);
    assert(content);
    assert(length >= 0);

    container_text(state, &state->container, content, length);
}
void ui_number(ui_state* state, u32 number)
{
    return;
    /*
    if (number == 0)
    {
        ui_text(state, "0", 1);
        return;
    }

    // Extract digits:
    fixed_array numberBuffer = { 0 };
    fixed_array_init(&numberBuffer, SCA_UI_MAX_NUMBER_LENGTH);

    char temp[SCA_UI_MAX_NUMBER_LENGTH];
    u8 length = number_to_array(number, temp);
    for (i32 i = length - 1; i >= 0; --i)
        fixed_array_push(&numberBuffer, &temp[i], 1);

    // Render:
    ui_text(state, numberBuffer.buffer, numberBuffer.current);
    fixed_array_destroy(&numberBuffer);
    */
}
void ui_text_absolute(ui_state* state, u32 x, u32 y, const char* content, u32 length)
{
    memory_pool* pool = state->pool;

    /*
    float* color = (float*)scarce_push(pool, sizeof(float) * 3);
    float* background = (float*)scarce_push(pool, sizeof(float) * 3);
    color = get_color_with_flags(state->color.color, state->color.colorIntense, state->color.colorFaint);
    background = get_color_with_flags(state->color.background, state->color.backgroundIntense, state->color.backgroundFaint);

    for(u32 i = 0; i < length; ++i)
    {
        if(content[i] == '\0')
            break;

        if (x + i > text_renderer_width(state->renderer))
            break;

        text_renderer_set_character_letter(state->renderer, x + i, y, content[i]);
        text_renderer_set_character_color(state->renderer, x + i, y, color[0], color[1], color[2]);
        
        text_renderer_set_character_background_color
        (
            state->renderer, x + i, y, 
            background[0], background[1], background[2], 
            state->color.renderBackground
        );
    }
    
    scarce_pop(pool, sizeof(float) * 6);
    */
}

void ui_set_align(ui_state* state, text_align align, u16 xOffset)
{
    assert(state);
    container_set_align(&state->container, align, xOffset);
}
void ui_set_position(ui_state* state, text_position position, u16 yOffset)
{
    assert(state);
    container_set_position(&state->container, position, yOffset);
}
void ui_set_color(ui_state* state, text_color* color)
{
    assert(state);
    state->color = *color;
}

void ui_sameline(ui_state* state, bool sameLine)
{
    assert(state);
    state->container.sameline = sameLine;
}
void ui_feed(ui_state* state)
{
    assert(state);
    container_feed(&state->container);
}
void ui_nudge(ui_state* state, u32 xOffset)
{
    assert(state);
    container_nudge(&state->container, xOffset);
}
void ui_space(ui_state* state, u32 yOffset)
{
    assert(state);
    container_space(&state->container, yOffset);
}
void ui_hline(ui_state* state, char lineChar)
{
    assert(state);

    float* color = get_color_with_flags(state->color.color, state->color.colorIntense, state->color.colorFaint);
    float* background = get_color_with_flags(state->color.background, state->color.backgroundIntense, state->color.backgroundFaint);

    container* container = &state->container;
    if (container->currentY > container->height)
        return;

    container->prevY = container->currentY;

    for (u32 x = 0; x < text_renderer_width(state->renderer); ++x)
    {
        i16 y = *container_determine_y_from_position(container, text_renderer_height(state->renderer)); 

        if (container_handle_y_overflow(container))
            break;

        text_renderer_set_character_letter(state->renderer, x, y, lineChar);
        text_renderer_set_character_color(state->renderer, x, y, color[0], color[1], color[2]);
        text_renderer_set_character_background_color(state->renderer, x, y, background[0], background[1], background[2], state->color.renderBackground);
    }

    container_space(&state->container, 1);
}

void ui_switch_container(ui_state* state, container* newContainer)
{
    assert(state);
    assert(newContainer);
    
    state->defaultContainer = false;
    state->prevContainer = state->container;
    
    container_fix_offset_bounds(newContainer, text_renderer_width(state->renderer), text_renderer_height(state->renderer));
    state->container = *newContainer;
}
void ui_restore_container(ui_state* state)
{
    assert(state);

    state->container = state->prevContainer;
    container_reset(&state->prevContainer);
}

aabb ui_mouse_aabb(engine* e)
{
    assert(e);

    aabb mouseAABB = { 0 };
    {
        u32 mouseX;
        u32 mouseY;

        text_renderer_get_mouse_grid_position(e->window, e->renderer, &mouseX, &mouseY);
        mouseAABB.x = mouseX;
        mouseAABB.y = mouseY;
        mouseAABB.width = 1;
        mouseAABB.height = 1;
    }

    return mouseAABB;
}