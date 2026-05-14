#include "ui.h"
#include "core/memory/memory.h"
#include <stdbool.h>
#include "engine.h"
#include "fixed_array.h"
#include "memory/stack.h"
#include "physics/aabb.h"
#include "string_utils.h"
#include "text_renderer.h"

#include "scarce.h"

static float* get_color_with_flags(u32 symbolColor, bool isIntense, bool isFaint)
{
    static float colors[][3] = 
    {
        { 0.141f, 0.161f, 0.180f }, // Black
        { 0.933f, 0.275f, 0.282f }, // Red
        { 0.165f, 0.631f, 0.596f }, // Green
        { 0.961f, 0.686f, 0.208f }, // Yellow
        { 0.231f, 0.447f, 0.702f }, // Blue
        { 0.612f, 0.404f, 0.631f }, // Magenta
        { 0.106f, 0.588f, 0.725f }, // Cyan
        { 0.937f, 0.941f, 0.945f }  // White
    };

    static float colors_intense[][3] = 
    {
        { 0.306f, 0.341f, 0.384f }, // Bright Black
        { 0.996f, 0.380f, 0.380f }, // Bright Red
        { 0.110f, 0.925f, 0.612f }, // Bright Green
        { 0.996f, 0.816f, 0.298f }, // Bright Yellow
        { 0.365f, 0.592f, 0.847f }, // Bright Blue
        { 0.757f, 0.525f, 0.773f }, // Bright Magenta
        { 0.106f, 0.796f, 0.933f }, // Bright Cyan
        { 0.992f, 0.996f, 0.996f }  // Bright White
    };

    static float faint_result[3];

    u32 index = (symbolColor > SY_COLOR_WHITE) ? SY_COLOR_WHITE : symbolColor;

    float* selected = (isIntense) ? colors_intense[index] : colors[index];
    if (isFaint) 
    {
        faint_result[0] = selected[0] * 0.5f;
        faint_result[1] = selected[1] * 0.5f;
        faint_result[2] = selected[2] * 0.5f;
        return faint_result;
    }

    return selected;
}

static u32 render_get_center(u32 screenWidth, u32 length)
{
    return screenWidth / 2 - length / 2;
}

static void ui_reset(ui_state* state)
{
    state->alignment = UI_ALIGN_CENTER;
    state->positioning = UI_POS_NONE;

    state->color.color = SY_COLOR_NONE;
    state->color.colorIntense = false;
    state->color.colorFaint = false;

    state->color.background = SY_COLOR_NONE;
    state->color.backgroundIntense = false;
    state->color.backgroundFaint = false;

    state->color.renderBackground = false;

    state->x = 0;
    state->y = 0;
    state->prevX = 0;
    state->prevY = 0;
    state->overflow = false;
    state->sameLine = false;
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

    ui_reset(state);
    return state;
}
void ui_end(ui_state* state)
{
    scarce_pop(state->pool, sizeof(ui_state));
}
void ui_clear(engine* e)
{
    text_renderer_zero_buffer(e->renderer);
}

void ui_text(ui_state* state, const char* content, u32 length)
{
    memory_pool* pool = state->pool;
    state->overflow = false;

    float* color = (float*)scarce_push(pool, sizeof(float) * 3);
    float* background = (float*)scarce_push(pool, sizeof(float) * 3);
    color = get_color_with_flags(state->color.color, state->color.colorIntense, state->color.colorFaint);
    background = get_color_with_flags(state->color.background, state->color.backgroundIntense, state->color.backgroundFaint);
    
    switch (state->alignment)
    {
        case UI_ALIGN_CENTER:
            state->x += render_get_center(text_renderer_width(state->renderer), length);
        break;

        case UI_ALIGN_LEFT:
            break;

        case UI_ALIGN_RIGHT:
            state->x = text_renderer_width(state->renderer) - state->x - length;
            break;
    }

    switch (state->positioning)
    {
        case UI_POS_TOP:
        case UI_POS_NONE:
            break;
        
        case UI_POS_BOTTOM:
            state->y = text_renderer_height(state->renderer) - 1 - state->y;
            break;
    }

    state->prevX = state->x;
    state->prevY = state->y;

    for(u16 i = 0; i < length; ++i)
    {
        char symbol = content[i];
        if (symbol == '\0')
            break;

        if (symbol == '\n')
        {
            ++state->y;
            continue;
        }

        if (state->x >= (u16)text_renderer_width(state->renderer))
        {
            state->x = state->prevX;
            ++state->y;
        }

        if (state->prevX >= (u16)text_renderer_width(state->renderer))
        {
            state->xOverflow = true;
            break;
        }

        if (state->y >= (u16)text_renderer_height(state->renderer))
        {
            state->overflow = true;
            break;
        }

        text_renderer_set_character_letter(state->renderer, state->x, state->y, content[i]);
        text_renderer_set_character_color(state->renderer, state->x, state->y, color[0], color[1], color[2]);
        
        text_renderer_set_character_background_color
        (
            state->renderer, state->x, state->y, 
            background[0], background[1], background[2], 
            state->color.renderBackground
        );
    
        state->x++;
    }
    
    if(!state->sameLine)
        state->y += 1;

    scarce_pop(pool, sizeof(float) * 3);
    scarce_pop(pool, sizeof(float) * 3);
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
    fixed_array_init(&numberBuffer, SCA_UI_MAX_NUMBER_LENGTH);

    char temp[SCA_UI_MAX_NUMBER_LENGTH];
    u8 length = number_to_array(number, temp);
    for (i32 i = length - 1; i >= 0; --i)
        fixed_array_push(&numberBuffer, &temp[i], 1);

    // Render:
    ui_text(state, numberBuffer.buffer, numberBuffer.current);
    fixed_array_destroy(&numberBuffer);
}
void ui_text_absolute(ui_state* state, u32 x, u32 y, const char* content, u32 length)
{
    memory_pool* pool = state->pool;

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
}

void ui_set_align(ui_state* state, text_align align, u16 xOffset)
{
    state->alignment = align;
    state->x = xOffset;
    state->alignOffset = xOffset;
}
void ui_set_position(ui_state* state, text_position position, u16 yOffset)
{
    state->positioning = position;
    state->y = yOffset;
}
void ui_set_color(ui_state* state, text_color* color)
{
    state->color = *color;
}

void ui_sameline(ui_state* state, bool sameLine)
{
    state->sameLine = sameLine;
}
void ui_feed(ui_state* state)
{
    state->x = state->alignOffset;
}
void ui_nudge(ui_state* state, u32 xOffset)
{
    state->x += xOffset;
}
void ui_space(ui_state* state, u32 yOffset)
{
    state->y += yOffset;
}
void ui_hline(ui_state* state, u32 y, char lineChar)
{
    memory_pool* pool = state->pool;

    float* color = (float*)scarce_push(pool, sizeof(float) * 3);
    float* background = (float*)scarce_push(pool, sizeof(float) * 3);
    color = get_color_with_flags(state->color.color, state->color.colorIntense, state->color.colorFaint);
    background = get_color_with_flags(state->color.background, state->color.backgroundIntense, state->color.backgroundFaint);

    for (u32 x = 0; x < text_renderer_width(state->renderer); ++x)
    {
        text_renderer_set_character_letter(state->renderer, x, y, lineChar);
        text_renderer_set_character_color(state->renderer, x, y, color[0], color[1], color[2]);

        text_renderer_set_character_background_color
        (
            state->renderer, x, y, 
            background[0], background[1], background[2], 
            state->color.renderBackground
        );
    }

    state->prevY = y;

    scarce_pop(pool, sizeof(float) * 3);
    scarce_pop(pool, sizeof(float) * 3);
}

aabb ui_mouse_aabb(engine* e)
{
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