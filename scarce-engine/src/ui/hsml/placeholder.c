#include "placeholder.h"
#include "core/dynamic_array.h"
#include "core/logging/logger.h"
#include "core/string_utils.h"

#include "defines.h"
#include "platform/platform.h"
#include "text_renderer.h"
#include <ctype.h>

typedef u32 (*placeholder_value_fetcher)(const ui_state* const state);
struct
{
    const char* placeholderName;
    placeholder_value_fetcher fetcher;
} typedef placeholder_table_item;

static u32 placeholder_overflow_fetcher(const ui_state* const state)
{
    return state->overflow;
}

static u32 placeholder_width_fetcher(const ui_state* const state)
{
    return text_renderer_width(state->renderer);
}

static u32 placeholder_height_fetcher(const ui_state* const state)
{
    return text_renderer_height(state->renderer);
}

static u32 placeholder_x_fetcher(const ui_state* const state)
{
    return state->x;
}

static u32 placeholder_y_fetcher(const ui_state* const state)
{
    return state->y;
}

static u32 placeholder_prev_x_fetcher(const ui_state* const state)
{
    return state->prevX;
}

static u32 placeholder_prev_y_fetcher(const ui_state* const state)
{
    return state->prevY;
}

static u32 placeholder_mouse_x_fetcher(const ui_state* const state)
{
    u32 x, y;
    text_renderer_get_mouse_grid_position(state->renderer->window, state->renderer, &x, &y);
    return x;
}
static u32 placeholder_mouse_y_fetcher(const ui_state* const state)
{
    u32 x, y;
    text_renderer_get_mouse_grid_position(state->renderer->window, state->renderer, &x, &y);
    return y;
}

static placeholder_table_item gPlaceholderTable[] =
{
    { "overflow", placeholder_overflow_fetcher },

    { "width", placeholder_width_fetcher },
    { "w", placeholder_width_fetcher },
    { "height", placeholder_height_fetcher },
    { "h", placeholder_height_fetcher },

    { "x", placeholder_x_fetcher },
    { "y", placeholder_y_fetcher },
    { "prevX", placeholder_prev_x_fetcher },
    { "prevY", placeholder_prev_y_fetcher },

    { "mouseX", placeholder_mouse_x_fetcher },
    { "mouseY", placeholder_mouse_y_fetcher },
};

u32 hsml_fetch_placeholder_value(ui_state* state, file_descriptor descriptor)
{
    memory_pool* pool = state->pool;
    
    u8 userPlaceholderAmount = pool[SCA_STACK_ADDRESS + pool[SCA_STACK_SIZE_ADDRESS] - 1];
    bool placeholderValueFound = false;

    dynamic_array buffer = { 0 };
    dynamic_array_init(&buffer, 1, sizeof(char));

    bool valueFound = false;
    while (true)
    {
        char next;
        if (!platform_read_file(descriptor, &next, 1) || next == '\n' || next == HSML_TOKEN_DELIMITER || isspace(next))
        {
            platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
            break;
        }

        valueFound = true;
        dynamic_array_push(&buffer, &next, 1);
    }

    if (!valueFound)
        log_critical_s("Invalid HSML file: invalid placeholder", 39);

    u32 placeholderValue = invalid_handle;
    u32 itemCount = sizeof(gPlaceholderTable) / sizeof(placeholder_table_item);
    for (u32 i = 0; i < itemCount; ++i)
    {
        placeholder_table_item* current = &gPlaceholderTable[i];
        if (is_string_same(current->placeholderName, buffer.buffer, buffer.current))
        {
            placeholderValue = current->fetcher(state);
            placeholderValueFound = true;
            break;
        }
    }
    
    u32 validDigits = partial_string_number(buffer.buffer, buffer.current);
    if (placeholderValue == invalid_handle && validDigits > 0)
    {
        placeholderValue = 0;

        u32 index = 0;
        char* bufferData = (char*)buffer.buffer;
        for (u32 i = 0; i < buffer.current; ++i)
        {
            char symbol = bufferData[i];
            index = (index * 10) + (symbol - '0');
        }

        placeholderValue = (u32)(pool[SCA_STACK_ADDRESS + pool[SCA_STACK_SIZE_ADDRESS] - userPlaceholderAmount + index - 1]);
        placeholderValueFound = true;

        platform_file_seek(descriptor, SEEK_MODE_CURRENT, -(buffer.current - validDigits));
    }

    if (!placeholderValueFound)
        log_critical_s("Invalid HSML file: invalid placeholder", 39);

    dynamic_array_destroy(&buffer);
    return placeholderValue;
}