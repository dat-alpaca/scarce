#include "ui.h"
#include "batch_renderer.h"
#include "cglm/vec4.h"
#include "core/memory/memory.h"
#include <stdbool.h>
#include "engine.h"
#include "memory/stack.h"
#include "memory/tag.h"
#include "physics/aabb.h"
#include "string_utils.h"

#include "graphics/batch_renderer.h"
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

void ui_begin(ui_state* state, memory_pool* pool, batch_renderer* renderer)
{
    assert(state);
    assert(pool);
    assert(renderer);

    state->pool = pool;
    state->renderer = renderer;

    ui_reset(state);
}
ui_state* ui_begin_stack(memory_pool* pool, batch_renderer* renderer)
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
    batch_renderer_zero_buffer(e->renderer);
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
    if (number == 0)
    {
        ui_text(state, "0", 1);
        return;
    }

    // Extract digits:
    fixed_array numberBuffer = { 0 };
    fixed_array_init(&numberBuffer, SCA_UI_MAX_NUMBER_LENGTH, TAG_TRANSIENT);

    char temp[SCA_UI_MAX_NUMBER_LENGTH];
    u64 length = number_to_array(number, temp);
    for (i32 i = length - 1; i >= 0; --i)
        fixed_array_push(&numberBuffer, &temp[i], 1);

    // Render:
    ui_text(state, numberBuffer.buffer, numberBuffer.current);
    fixed_array_destroy(&numberBuffer);
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

    vec4 color, background;
    get_color_with_flags(state->color.color, state->color.colorIntense, state->color.colorFaint, color);
    get_color_with_flags(state->color.color, state->color.colorIntense, state->color.colorFaint, background);

    container* container = &state->container;
    if (container->currentY > container->height)
        return;

    container->prevY = container->currentY;

    for (u32 x = container->_x; x < container->_x + state->renderer->gridWidth; ++x)
    {
        i16 y = *container_determine_y_from_position(container, state->renderer->gridHeight); 

        if (container_handle_y_overflow(container))
            break;

        batch_renderer_cell cell = { 0 };
        glm_vec4_copy(background, cell.backgroundColor);
        glm_vec4_copy(color, cell.color);
        cell.layer = lineChar;

        batch_renderer_set_cell(state->renderer, &cell, x, y);
    }

    container_space(&state->container, 1);
}

void ui_vline(ui_state* state, u32 x, char lineChar)
{
    assert(state);

    vec4 color, background;
    get_color_with_flags(state->color.color, state->color.colorIntense, state->color.colorFaint, color);
    get_color_with_flags(state->color.color, state->color.colorIntense, state->color.colorFaint, background);

    container* container = &state->container;
    if (x >= container->width)
        x = container->width - 1;

    container->prevX = container->currentX;

    for (u32 y = container->_y; y < container->_y + state->renderer->gridHeight; ++y)
    {
        batch_renderer_cell cell = { 0 };
        glm_vec4_copy(background, cell.backgroundColor);
        glm_vec4_copy(color, cell.color);
        cell.layer = lineChar;
        batch_renderer_set_cell(state->renderer, &cell, x, y);
    }
}

void ui_switch_container(ui_state* state, container* newContainer)
{
    assert(state);
    assert(newContainer);
    
    state->defaultContainer = false;
    state->prevContainer = state->container;
    
    container_fix_offset_bounds(newContainer, state->renderer->gridWidth, state->renderer->gridHeight);
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

        batch_renderer_get_mouse_grid_position(e->window, e->renderer, &mouseX, &mouseY);
        mouseAABB.x = mouseX;
        mouseAABB.y = mouseY;
        mouseAABB.width = 1;
        mouseAABB.height = 1;
    }

    return mouseAABB;
}