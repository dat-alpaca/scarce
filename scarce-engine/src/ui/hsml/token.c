#include "token.h"

#include "platform/platform.h"
#include "dynamic_array.h"
#include "logging/logger.h"
#include "string_utils.h"
#include "ui/hsml/conditional.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "defines.h"
#include "ui/hsml/placeholder.h"
#include "ui/ui.h"

// Fetching:
static void hsml_fetch_text(ui_state* state, file_descriptor descriptor, dynamic_array* buffer, char delimiter, bool ignoreSpace, bool allLower)
{
    dynamic_array_init(buffer, 32, sizeof(char));

    while (true)
    {
        char next;
        if (!platform_read_file(descriptor, &next, 1) || next == '\n')
        {
            remove_trailing_space(buffer);
            break;
        }

        if (next == HSML_TOKEN_PLACEHOLDER)
        {
            hsml_token placeholder = hsml_fetch_placeholder_value(state, descriptor);
            
            if (placeholder.type == HSML_TOKEN_TEXT)
            {
                char* data = (char*)placeholder.value.buffer;
                for (u32 i = 0; i < placeholder.value.current; ++i)
                {
                    char current = data[i];
                    dynamic_array_push(buffer, &current, 1);
                }
            }
            else
            {
                u64 value = hsml_fetch_number_from_placeholder(&placeholder);
                number_to_buffer(value, buffer);
            }

            dynamic_array_destroy(&placeholder.value);
            continue;
        }

        if (delimiter && next == delimiter)
        {
            remove_trailing_space(buffer);
            break;
        }

        if (ignoreSpace && next == ' ')
            continue;

        if (allLower)
            next = tolower(next);

        dynamic_array_push(buffer, &next, 1);
    }

    char null = '\0';
    dynamic_array_push(buffer, &null, 1);
}

static u32 hsml_fetch_number(ui_state* state, file_descriptor descriptor)
{
    dynamic_array buffer = { 0 };
    dynamic_array_init(&buffer, 16, sizeof(char));

    while (true)
    {
        char next;
        if (!platform_read_file(descriptor, &next, 1))
            return HSML_TOKEN_INVALID;

        if (next == HSML_TOKEN_PLACEHOLDER)
        {
            hsml_token placeholder = hsml_fetch_placeholder_value(state, descriptor);
            if (placeholder.type == HSML_TOKEN_TEXT)
                log_critical_s("Invalid HSML: expected value", 29);
            
            u64 value = hsml_fetch_number_from_placeholder(&placeholder);
            
            dynamic_array_destroy(&placeholder.value);
            dynamic_array_destroy(&buffer);
            return value;
        }

        if (next == ' ')
            continue;

        if (!isdigit(next))
            break;

        dynamic_array_push(&buffer, &next, 1);
    }

    if (buffer.current <= 0)
    {
        log_critical_s("Invalid HSML file: delimiter missing", 37);
        return 0;
    }

    char* endptr;
    u32 num = (u32)strtol(buffer.buffer, &endptr, 10);

    if (buffer.buffer == endptr)
    {
        log_critical_s("Invalid HSML file: no digits found", 35);
        return 0;
    }

    dynamic_array_destroy(&buffer);
    return num;
}

static char hsml_fetch_character(file_descriptor descriptor)
{
    dynamic_array buffer = { 0 };
    dynamic_array_init(&buffer, 16, sizeof(char));

    char symbol;
    while(true)
    {
        if(!platform_read_file(descriptor, &symbol, 1))
            log_critical_s("Invalid HSML: invalid hline parameters", 43);
        if (symbol != ' ')
            break;
    }

    dynamic_array_destroy(&buffer);
    return symbol;
}

static hsml_token_color_item s_colorLookupTable[] = 
{
    { "default", HSML_COLOR_DEFAULT },
    { "none", HSML_COLOR_NONE },
    { "red", HSML_COLOR_RED },
    { "green", HSML_COLOR_GREEN },
    { "yellow", HSML_COLOR_YELLOW },
    { "blue", HSML_COLOR_BLUE },
    { "purple", HSML_COLOR_PURPLE },
    { "cyan", HSML_COLOR_CYAN },
    { "white", HSML_COLOR_WHITE },

    { "intense", HSML_COLOR_INTENSE },
    { "not_intense", HSML_COLOR_NOT_INTENSE },
    { "nintense", HSML_COLOR_NOT_INTENSE },
    { "ni", HSML_COLOR_NOT_INTENSE },

    { "faint", HSML_COLOR_FAINT },
    { "not_faint", HSML_COLOR_NOT_FAINT },
    { "nfaint", HSML_COLOR_NOT_FAINT },
    { "nf", HSML_COLOR_NOT_FAINT },
};

static hsml_color_token_type hsml_parse_color(dynamic_array* text)
{
    hsml_color_token_type color = HSML_COLOR_INVALID;

    for (hsml_token_color_item* item = s_colorLookupTable; item->name != NULL; ++item)
    {
        if (strcmp(item->name, text->buffer) == 0)
            color = item->color;
    }

    return color;
}

static void hsml_fetch_colors(file_descriptor descriptor, dynamic_array* colors)
{
    char null = '\0';
    dynamic_array_init(colors, 3, sizeof(hsml_color_token_type));

    dynamic_array text = { 0 };
    dynamic_array_init(&text, 16, sizeof(char));
    while (true)
    {
        char next;
        if (!platform_read_file(descriptor, &next, 1) || next == '\n' || next == HSML_TOKEN_DELIMITER)
        {
            dynamic_array_push(&text, &null, 1);
            hsml_color_token_type colorToken = hsml_parse_color(&text);
            
            dynamic_array_push(colors, &colorToken, 1);
            dynamic_array_clear(&text);
            break;
        }

        if (next == ' ')
        {
            if (text.current <= 0)
                continue;

            dynamic_array_push(&text, &null, 1);
            hsml_color_token_type colorToken = hsml_parse_color(&text);

            dynamic_array_push(colors, &colorToken, 1);
            dynamic_array_clear(&text);
            continue;
        }

        next = tolower(next);
        dynamic_array_push(&text, &next, 1);
    }

    dynamic_array_destroy(&text);
}

static bool hsml_fetch_condition(ui_state* state, file_descriptor descriptor)
{
    return hsml_get_conditional_result(state, descriptor);
}

// Token lookup:
static hsml_token_lookup_item s_tokenLookupTable[] = 
{
    { "include", HSML_TOKEN_INCLUDE },
    { "button", HSML_TOKEN_BUTTON },
    { "textbox", HSML_TOKEN_TEXTBOX },
    
    { "container", HSML_TOKEN_CONTAINER },
    { "endcontainer", HSML_TOKEN_CONTAINER_END },

    { "if", HSML_TOKEN_IF },
    { "endif", HSML_TOKEN_END_IF },

    { "start", HSML_TOKEN_START },
    { "end", HSML_TOKEN_END },

    { "feed", HSML_TOKEN_FEED },
    { "f", HSML_TOKEN_FEED },
    { "feedline", HSML_TOKEN_FEEDLINE },
    { "fl", HSML_TOKEN_FEEDLINE },
    
    { "center", HSML_TOKEN_ALIGN_CENTER },
    { "left", HSML_TOKEN_ALIGN_LEFT },
    { "right", HSML_TOKEN_ALIGN_RIGHT },

    { "none", HSML_TOKEN_POS_NONE },
    { "top", HSML_TOKEN_POS_TOP },
    { "bottom", HSML_TOKEN_POS_BOTTOM },

    { "hline", HSML_TOKEN_HLINE },
    { "space", HSML_TOKEN_SPACE },
    { "nudge", HSML_TOKEN_NUDGE },

    { "sameline", HSML_TOKEN_SAMELINE },
    { "sl", HSML_TOKEN_SAMELINE },

    { "color", HSML_TOKEN_COLOR },
    { "bgcolor", HSML_TOKEN_BG_COLOR },
    { "background_color", HSML_TOKEN_BG_COLOR },

    { "background", HSML_TOKEN_BG },
    { "bg", HSML_TOKEN_BG },
};

static hsml_token_type hsml_get_token_type(ui_state* state, file_descriptor descriptor)
{
    dynamic_array buffer = { 0 };
    hsml_fetch_text(state, descriptor, &buffer, HSML_TOKEN_DELIMITER, false, true);

    if (!buffer.current)
        return HSML_TOKEN_INVALID;

    hsml_token_type type = HSML_TOKEN_INVALID;
    for (hsml_token_lookup_item* item = s_tokenLookupTable; item->name != NULL; ++item)
    {
        if (strcmp(item->name, buffer.buffer) == 0)
            type = item->type;
    }
    
    dynamic_array_destroy(&buffer);
    return type;
}

static hsml_token hsml_create_token(ui_state* state, hsml_token_type type, file_descriptor descriptor)
{
    hsml_token token = { 0 };
    token.type = type;

    switch(type)
    {
        // conditionals:
        case HSML_TOKEN_IF:
        {
            u32 condition = (u32)hsml_fetch_condition(state, descriptor);
            dynamic_array_init(&token.value, 1, sizeof(u32));
            dynamic_array_push(&token.value, &condition, 1);
        } break;
        case HSML_TOKEN_END_IF:
            break;

        // Requires no arguments:
        case HSML_TOKEN_START: 
        case HSML_TOKEN_END:
        case HSML_TOKEN_FEED:
        case HSML_TOKEN_FEEDLINE:
            break;

        // Requires color arguments:
        case HSML_TOKEN_COLOR:
        case HSML_TOKEN_BG_COLOR:
            hsml_fetch_colors(descriptor, &token.value);
            break;

        // requires text argument
        case HSML_TOKEN_TEXT:
            hsml_fetch_text(state, descriptor, &token.value, HSML_TOKEN_SYMBOL, false, false);
            break;

        case HSML_TOKEN_INCLUDE:
            hsml_fetch_text(state, descriptor, &token.value, HSML_TOKEN_DELIMITER, true, false);
            break;
    
        // Requires numerical parameter:
        case HSML_TOKEN_SAMELINE:   
        
        case HSML_TOKEN_ALIGN_CENTER:
        case HSML_TOKEN_ALIGN_LEFT:
        case HSML_TOKEN_ALIGN_RIGHT:

        case HSML_TOKEN_POS_NONE:
        case HSML_TOKEN_POS_TOP:
        case HSML_TOKEN_POS_BOTTOM:

        case HSML_TOKEN_SPACE:
        case HSML_TOKEN_NUDGE:

        case HSML_TOKEN_BG:
        {
            u32 number = hsml_fetch_number(state, descriptor);
            
            dynamic_array_init(&token.value, 1, sizeof(u32));
            dynamic_array_push(&token.value, &number, 1);
        } break;

        // requires multiple arguments:
        case HSML_TOKEN_TEXTBOX:
        {
            u8 baseAddress = hsml_fetch_number(state, descriptor);
            u8 index = hsml_fetch_number(state, descriptor);
            
            dynamic_array_init(&token.value, 2, sizeof(u8));
            dynamic_array_push(&token.value, &baseAddress, 1);
            dynamic_array_push(&token.value, &index, 1);
        } break;

        case HSML_TOKEN_CONTAINER:
        {
            u8 x = hsml_fetch_number(state, descriptor);
            u8 y = hsml_fetch_number(state, descriptor);
            u8 w = hsml_fetch_number(state, descriptor);
            u8 h = hsml_fetch_number(state, descriptor);
            
            dynamic_array_init(&token.value, 4, sizeof(u8));
            dynamic_array_push(&token.value, &x, 1);
            dynamic_array_push(&token.value, &y, 1);
            dynamic_array_push(&token.value, &w, 1);
            dynamic_array_push(&token.value, &h, 1);
        } break;
        case HSML_TOKEN_CONTAINER_END: 
            break;

        case HSML_TOKEN_BUTTON:
        {
            u8 baseAddress = hsml_fetch_number(state, descriptor);
            u8 index = hsml_fetch_number(state, descriptor);
            
            dynamic_array buffer;
            dynamic_array_init(&buffer, 16, sizeof(char));
            hsml_fetch_text(state, descriptor, &buffer, HSML_TOKEN_DELIMITER, true, false);
            
            dynamic_array_init(&token.value, 2 + buffer.current, sizeof(u8));
            dynamic_array_push(&token.value, &baseAddress, 1);
            dynamic_array_push(&token.value, &index, 1);
            dynamic_array_push(&token.value, buffer.buffer, buffer.current);

            dynamic_array_destroy(&buffer);
        } break;

        case HSML_TOKEN_HLINE:
        {
            u32 character = (u32)hsml_fetch_character(descriptor);
            
            dynamic_array_init(&token.value, 2, sizeof(u32));
            dynamic_array_push(&token.value, &character, 1);
        } break;

        default:
            log_critical_s("Invalid HSML: invalid token type", 33);
    }

    return token;
}

hsml_token_argument hsml_get_argument_type(hsml_token_type type)
{
    switch(type)
    {
        case HSML_TOKEN_START: 
        case HSML_TOKEN_END:
        case HSML_TOKEN_FEED:
        case HSML_TOKEN_FEEDLINE:
            return HSML_TOKEN_ARG_NONE;

        case HSML_TOKEN_COLOR:
        case HSML_TOKEN_BG_COLOR:
            return HSML_TOKEN_ARG_COLOR;

        case HSML_TOKEN_INCLUDE:
            return HSML_TOKEN_ARG_TEXT;

        // stores the result of the conditional expression
        case HSML_TOKEN_IF:
            return HSML_TOKEN_ARG_NUMERIC;
        case HSML_TOKEN_END_IF:
            return HSML_TOKEN_ARG_NONE;

        // container
        case HSML_TOKEN_CONTAINER:
            return HSML_TOKEN_ARG_MULTIPLE;
        case HSML_TOKEN_CONTAINER_END:
            return HSML_TOKEN_ARG_NONE;

        case HSML_TOKEN_SAMELINE:   
        case HSML_TOKEN_ALIGN_CENTER:
        case HSML_TOKEN_ALIGN_LEFT:
        case HSML_TOKEN_ALIGN_RIGHT:
        case HSML_TOKEN_POS_NONE:
        case HSML_TOKEN_POS_TOP:
        case HSML_TOKEN_POS_BOTTOM:
        case HSML_TOKEN_SPACE:
        case HSML_TOKEN_NUDGE:
        case HSML_TOKEN_BG:
            return HSML_TOKEN_ARG_NUMERIC;

        case HSML_TOKEN_TEXTBOX:
        case HSML_TOKEN_BUTTON:
            return HSML_TOKEN_ARG_MULTIPLE;

        case HSML_TOKEN_HLINE:
            return HSML_TOKEN_ARG_CHAR;

        default:
            return HSML_TOKEN_ARG_INVALID;
    }

    return HSML_TOKEN_ARG_INVALID;
}

void hsml_tokenize(ui_state* state, const char* filepath, dynamic_array* tokens)
{
    file_descriptor descriptor = platform_open_file(filepath, SCA_FILE_READ);
    if (descriptor == invalid_file_descriptor)
    {
        log_critical_s("Invalid filepath", 17);
        return;
    }

    dynamic_array_init(tokens, 256, sizeof(hsml_token));

    char symbol;
    hsml_mode mode = HSML_MODE_NORMAL;
    bool prefixSpaces = true;
    while(platform_read_file(descriptor, &symbol, 1))
    {
        switch(mode)
        {
            case HSML_MODE_TEXT:
            {
                if (prefixSpaces && (isspace(symbol) || symbol == '\n'))
                {
                    while(platform_read_file(descriptor, &symbol, 1) && (isspace(symbol) || symbol == '\n'));
                    prefixSpaces = false;
                }

                platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
                hsml_token token = hsml_create_token(state, HSML_TOKEN_TEXT, descriptor);
                dynamic_array_push(tokens, &token, 1);
                
                mode = HSML_MODE_NORMAL;
                platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
            } break;

            case HSML_MODE_NORMAL:
            {
                if (isspace(symbol))
                    continue;

                if (symbol == HSML_TOKEN_COMMENT)
                    while(platform_read_file(descriptor, &symbol, 1) && symbol != '\n');

                else if (symbol == HSML_TOKEN_SYMBOL)
                {
                    hsml_token_type type = hsml_get_token_type(state, descriptor);
                    if (type == HSML_TOKEN_INVALID)
                        log_critical_s("Invalid HSML: invalid token type tokenized.", 44);

                    hsml_token token = hsml_create_token(state, type, descriptor);
                    dynamic_array_push(tokens, &token, 1);
                }

                else
                {
                    platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
                    mode = HSML_MODE_TEXT;
                }
            } break;
        }
    }

    platform_close_file(descriptor);
}