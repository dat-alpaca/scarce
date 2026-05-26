#include "placeholder.h"
#include "core/dynamic_array.h"
#include "core/logging/logger.h"
#include "core/string_utils.h"

#include "defines.h"
#include "platform/platform.h"
#include "batch_renderer.h"
#include "ui/hsml/token.h"

#include <ctype.h>
#include <string.h>

typedef u32 (*placeholder_value_fetcher)(const ui_state* const state);
struct
{
    const char* placeholderName;
    placeholder_value_fetcher fetcher;
} typedef placeholder_table_item;

static u32 placeholder_overflow_fetcher(const ui_state* const state)
{
    return state->container.overflow;
}

static u32 placeholder_width_fetcher(const ui_state* const state)
{
    return state->renderer->gridWidth;
}

static u32 placeholder_height_fetcher(const ui_state* const state)
{
    return state->renderer->gridHeight;
}

static u32 placeholder_x_fetcher(const ui_state* const state)
{
    return state->container.currentX;
}

static u32 placeholder_y_fetcher(const ui_state* const state)
{
    return state->container.currentY;
}

static u32 placeholder_prev_x_fetcher(const ui_state* const state)
{
    return state->container.prevX;
}

static u32 placeholder_prev_y_fetcher(const ui_state* const state)
{
    return state->container.prevY;
}

static u32 placeholder_mouse_x_fetcher(const ui_state* const state)
{
    u32 x, y;
    batch_renderer_get_mouse_grid_position(state->renderer->window, state->renderer, &x, &y);
    return x;
}
static u32 placeholder_mouse_y_fetcher(const ui_state* const state)
{
    u32 x, y;
    batch_renderer_get_mouse_grid_position(state->renderer->window, state->renderer, &x, &y);
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

hsml_token hsml_fetch_placeholder_value(ui_state* state, file_descriptor descriptor)
{
    hsml_token result;
    memory_pool* pool = state->pool;
    
    u8 userPlaceholderAmount = pool[SCA_STACK_ADDRESS + pool[SCA_STACK_SIZE_ADDRESS] - 1];
    bool placeholderValueFound = false;

    dynamic_array buffer = { 0 };
    dynamic_array_init(&buffer, 1, sizeof(char));

    hsml_placeholder_read_mode mode = HSML_PLACEHOLDER_READ_U8;
    bool valueFound = false;
    while (true)
    {
        char current;
        if (!platform_read_file(descriptor, &current, 1) || current == '\n' || current == HSML_TOKEN_DELIMITER || isspace(current))
        {
            platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);   
            break;
        }

        if (current == HSML_TOKEN_PLACEHOLDER_U16)
        {
            mode = HSML_PLACEHOLDER_READ_U16;
            continue;
        }
        if (current == HSML_TOKEN_PLACEHOLDER_U32)
        {
            mode = HSML_PLACEHOLDER_READ_U32;
            continue;
        }
        if (current == HSML_TOKEN_PLACEHOLDER_U64)
        {
            mode = HSML_PLACEHOLDER_READ_U64;
            continue;
        }
        if (current == HSML_TOKEN_PLACEHOLDER_STR)
        {
            mode = HSML_PLACEHOLDER_READ_STRING;
            continue;
        }

        valueFound = true;
        dynamic_array_push(&buffer, &current, 1);
    }

    if (!valueFound)
        log_critical_s("Invalid HSML file: invalid placeholder", 39);

    u64 placeholderValue = invalid_handle;
    u32 itemCount = sizeof(gPlaceholderTable) / sizeof(placeholder_table_item);
    for (u32 i = 0; i < itemCount; ++i)
    {
        placeholder_table_item* current = &gPlaceholderTable[i];
        if (is_string_same(current->placeholderName, buffer.buffer, buffer.current))
        {
            result.type = HSML_TOKEN_U64;
            placeholderValue = current->fetcher(state);
            placeholderValueFound = true;

            dynamic_array_init(&result.value, 1, sizeof(u64));
            dynamic_array_push(&result.value, &placeholderValue, 1);
            
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

        u64 address = SCA_STACK_ADDRESS + pool[SCA_STACK_SIZE_ADDRESS] - userPlaceholderAmount + index - 1;
        switch(mode)
        {
            case HSML_PLACEHOLDER_READ_U8:
            {
                placeholderValue = pool[address];
                dynamic_array_init(&result.value, 1, sizeof(u8));
                dynamic_array_push(&result.value, &placeholderValue, 1);
                
                result.type = HSML_TOKEN_U8; 
                placeholderValueFound = true;
            } break;
            case HSML_PLACEHOLDER_READ_U16:
            {
                placeholderValue = *(u16*)(&pool[address]);
                dynamic_array_init(&result.value, 1, sizeof(u16));
                dynamic_array_push(&result.value, &placeholderValue, 1);
                
                result.type = HSML_TOKEN_U16; 
                placeholderValueFound = true;
            } break;
            case HSML_PLACEHOLDER_READ_U32:
            {
                result.type = HSML_TOKEN_U32;
                placeholderValueFound = true;

                placeholderValue = *(u32*)(&pool[address]);
                dynamic_array_init(&result.value, 1, sizeof(u32));
                dynamic_array_push(&result.value, &placeholderValue, 1);
            } break;
            case HSML_PLACEHOLDER_READ_U64:
            {
                result.type = HSML_TOKEN_U64;                
                placeholderValueFound = true;

                placeholderValue = *(u64*)(&pool[address]);
                dynamic_array_init(&result.value, 1, sizeof(u64));
                dynamic_array_push(&result.value, &placeholderValue, 1);
            } break;
            case HSML_PLACEHOLDER_READ_STRING:
            {
                result.type = HSML_TOKEN_TEXT;
                placeholderValueFound = true;

                char* data = (char*)(&pool[address]);
                u8 length = data[0];
                
                dynamic_array_init(&result.value, length, sizeof(char));
                for (u8 i = 1; i < length + 1; ++i)
                    dynamic_array_push(&result.value, &data[i], 1);
            } break;
            
            default:
                break;
        }

        platform_file_seek(descriptor, SEEK_MODE_CURRENT, -(buffer.current - validDigits));
    }

    if (!placeholderValueFound)
        log_critical_s("Invalid HSML file: invalid placeholder", 39);

    dynamic_array_destroy(&buffer);
    return result;
}

u64 hsml_fetch_number_from_placeholder(hsml_token* token)
{
    void* data = token->value.buffer;

    switch(token->type)
    {
        case HSML_TOKEN_U8:
        {
            u8 number;
            memcpy(&number, data, sizeof(u8));
            return (u64)number;
        }

        case HSML_TOKEN_U16:
        {
            u16 number;
            memcpy(&number, data, sizeof(u16));
            return (u64)number;
        }
    
        case HSML_TOKEN_U32:
        {
            u32 number;
            memcpy(&number, data, sizeof(u32));
            return (u64)number;
        }
        
        case HSML_TOKEN_U64:
        {
            u64 number;
            memcpy(&number, data, sizeof(u64));
            return number;
        }
        
        default:
            log_critical_s("Invalid HSML: invalid placeholder type", 39);
    }
    
    return 0;
}