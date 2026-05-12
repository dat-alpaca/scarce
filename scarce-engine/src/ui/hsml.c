#include "hsml.h"

#include "dynamic_array.h"
#include "fixed_array.h"
#include "memory/memory.h"
#include "ui/ui.h"
#include "logging/logger.h"
#include "platform/platform.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>

#define SCA_HSML_MAX_COLOR_KEYWORDS 5
#define SCA_HSML_MAX_TEXT_CAP 256
#define SCA_HSML_MAX_TEXT_VALUE_LENGTH 3

enum hsml_mode
{
    HSML_MODE_NONE,
    HSML_MODE_TEXT,
    HSML_MODE_FORMAT,
    HSML_MODE_COMMENT,
};

static hsml_token get_next_token(file_descriptor descriptor)
{
    fixed_array buffer = { 0 };
    fixed_array_init(&buffer, 32 * sizeof(char));
    
    bool possibleValidToken = false;
    while(true)
    {
        if (buffer.current >= buffer.capacity - 1)
            break;

        char symbol;
        platform_read_file(descriptor, &symbol, 1);

        if (symbol == ';')
        {
            possibleValidToken = true;
            break;
        }

        if (symbol == ' ')
            continue;

        fixed_array_push(&buffer, &symbol, 1);
    }
    
    if (!possibleValidToken)
    {
        fixed_array_destroy(&buffer);
        return HSML_TOKEN_NONE;
    }

    hsml_token token = HSML_TOKEN_NONE;

    char null = '\0';
    fixed_array_push(&buffer, &null, 1);

    if (strcmp(buffer.buffer, "include") == 0)
        token = HSML_TOKEN_INCLUDE;
    if (strcmp(buffer.buffer, "ifn") == 0 || strcmp(buffer.buffer, "if_n") == 0)
        token = HSML_TOKEN_IFN;
    if (strcmp(buffer.buffer, "if") == 0)
        token = HSML_TOKEN_IF;
    if (strcmp(buffer.buffer, "endif") == 0)
        token = HSML_TOKEN_END_IF;

    if (strcmp(buffer.buffer, "start") == 0)
        token = HSML_TOKEN_START;
    if (strcmp(buffer.buffer, "end") == 0)
        token = HSML_TOKEN_END;

    if (strcmp(buffer.buffer, "center") == 0)
        token = HSML_TOKEN_ALIGN_CENTER;
    if (strcmp(buffer.buffer, "left") == 0)
        token = HSML_TOKEN_ALIGN_LEFT;
    if (strcmp(buffer.buffer, "right") == 0)
        token = HSML_TOKEN_ALIGN_RIGHT;

    if (strcmp(buffer.buffer, "none") == 0)
        token = HSML_TOKEN_POS_NONE;
    if (strcmp(buffer.buffer, "top") == 0)
        token = HSML_TOKEN_POS_TOP;
    if (strcmp(buffer.buffer, "bottom") == 0)
        token = HSML_TOKEN_POS_BOTTOM;

    if (strcmp(buffer.buffer, "hline") == 0)
        token = HSML_TOKEN_HLINE;

    if (strcmp(buffer.buffer, "space") == 0)
        token = HSML_TOKEN_SPACE;
    if (strcmp(buffer.buffer, "nudge") == 0)
        token = HSML_TOKEN_NUDGE;
    if (strcmp(buffer.buffer, "feed") == 0)
        token = HSML_TOKEN_FEED;
    if (strcmp(buffer.buffer, "sl") == 0 || strcmp(buffer.buffer, "sameline") == 0)
        token = HSML_TOKEN_SAMELINE;

    if (strcmp(buffer.buffer, "color") == 0)
        token = HSML_TOKEN_COLOR;
    if (strcmp(buffer.buffer, "bgcolor") == 0 || strcmp(buffer.buffer, "background") == 0)
        token = HSML_TOKEN_BG_COLOR;
    if (strcmp(buffer.buffer, "bg") == 0)
        token = HSML_TOKEN_BG;

    fixed_array_destroy(&buffer);
    return token;
}

static u32 get_numeric_token(file_descriptor descriptor)
{
    fixed_array buffer = { 0 };
    fixed_array_init(&buffer, (12 + 1) * sizeof(char));
    
    bool possibleValid = false;
    while(true)
    {
        if (buffer.current >= buffer.capacity - 1)
            break;

        char symbol;
        platform_read_file(descriptor, &symbol, 1);

        if (symbol == ' ')
            continue;

        if (symbol == ';')
        {
            possibleValid = true;
            break;
        }

        bool validNumber = false;
        if (symbol == '-' || symbol == '+')
            validNumber = true;

        for(u8 i = 0; i < 10; ++i)
        {
            if (symbol != '0' + i)
                continue;

            validNumber = true;
            break;
        }

        if (!validNumber)
        {
            log_critical_s("Invalid HSML file: numerical argument has non numeric digits", 61);
            return 0;
        }

        fixed_array_push(&buffer, &symbol, 1);
    }

    if (!possibleValid)
    {
        log_critical_s("Invalid HSML file: ';' missing", 31);
        return 0;
    }

    char* endptr;
    u32 num = (u32)strtol(buffer.buffer, &endptr, 10);

    if (buffer.buffer == endptr)
    {
        log_critical_s("Invalid HSML file: no digits found", 35);
        return 0;
    }

    fixed_array_destroy(&buffer);
    return num;
}

static char get_character_token(file_descriptor descriptor)
{
    char symbol = 0;
    
    u32 charactersRead = 0;
    while(true)
    {
        platform_read_file(descriptor, &symbol, 1);
        ++charactersRead;

        if (symbol == ';')
            break;

        if (charactersRead > 1 && symbol == 0)
            log_critical_s("Invalid HSML file: Invalid char argument.", 42);
    }

    return symbol;
}

static void get_text_token(file_descriptor descriptor, dynamic_array* buffer)
{
    dynamic_array_init(buffer, 16, sizeof(char));

    bool possiblyValid = false;
    while(true)
    {
        char symbol = 0;
        platform_read_file(descriptor, &symbol, 1);

        if (symbol == ' ')
            continue;

        if (symbol == '\n')
            break;

        if (symbol == ';')
        {
            possiblyValid = true;
            break;
        }

        dynamic_array_push(buffer, &symbol, 1);

        if (buffer->capacity > SCA_HSML_MAX_TEXT_CAP)
            break;
    }

    char null = '\0';
    dynamic_array_push(buffer, &null, 1);
    if (!possiblyValid)
        log_critical_s("Invalid HSML file: Include might not have closing ;", 52);
}

static hsml_color_token_type get_color_token(fixed_array* buffer)
{
    if (strcmp(buffer->buffer, "default") == 0)
        return HSML_COLOR_DEFAULT;

    if (strcmp(buffer->buffer, "none") == 0)
        return HSML_COLOR_NONE;

    if (strcmp(buffer->buffer, "red") == 0)
        return HSML_COLOR_RED;

    if (strcmp(buffer->buffer, "green") == 0)
        return HSML_COLOR_GREEN;

    if (strcmp(buffer->buffer, "yellow") == 0)
        return HSML_COLOR_YELLOW;

    if (strcmp(buffer->buffer, "blue") == 0)
        return HSML_COLOR_BLUE;

    if (strcmp(buffer->buffer, "purple") == 0)
        return HSML_COLOR_PURPLE;

    if (strcmp(buffer->buffer, "cyan") == 0)
        return HSML_COLOR_CYAN;

    if (strcmp(buffer->buffer, "white") == 0)
        return HSML_COLOR_WHITE;

    if (strcmp(buffer->buffer, "intense") == 0)
        return HSML_COLOR_INTENSE;
    if (strcmp(buffer->buffer, "not_intense") == 0 || strcmp(buffer->buffer, "nintense") == 0 || strcmp(buffer->buffer, "ni") == 0)
        return HSML_COLOR_NOT_INTENSE;

    if (strcmp(buffer->buffer, "faint") == 0)
        return HSML_COLOR_FAINT;
    if (strcmp(buffer->buffer, "not_faint") == 0 || strcmp(buffer->buffer, "nfaint") == 0 || strcmp(buffer->buffer, "nf") == 0)
        return HSML_COLOR_NOT_FAINT;

    return HSML_COLOR_INVALID;
}

static void handle_color_token(hsml_color_token_type token, text_color* color)
{
    switch(token)
    {
        case HSML_COLOR_DEFAULT:
        {
            color->color = SY_COLOR_WHITE;
            color->colorFaint = false;
            color->colorIntense = false;
        } break;

        case HSML_COLOR_NONE: color->color = SY_COLOR_NONE; break;
        case HSML_COLOR_RED: color->color = SY_COLOR_RED; break;
        case HSML_COLOR_GREEN: color->color = SY_COLOR_GREEN; break;
        case HSML_COLOR_YELLOW: color->color = SY_COLOR_YELLOW; break;
        case HSML_COLOR_BLUE: color->color = SY_COLOR_BLUE; break;
        case HSML_COLOR_PURPLE: color->color = SY_COLOR_PURPLE; break;
        case HSML_COLOR_CYAN: color->color = SY_COLOR_CYAN; break;
        case HSML_COLOR_WHITE: color->color = SY_COLOR_WHITE; break;

        case HSML_COLOR_INTENSE: color->colorIntense = true; break;
        case HSML_COLOR_FAINT: color->colorFaint = true; break;

        case HSML_COLOR_NOT_INTENSE: color->colorIntense = false; break;
        case HSML_COLOR_NOT_FAINT: color->colorFaint = false; break;
        default:
            return;
    }
}

static void handle_bg_color_token(hsml_color_token_type token, text_color* color)
{
    switch(token)
    {
        case HSML_COLOR_DEFAULT:
        {
            color->background = SY_COLOR_WHITE;
            color->backgroundFaint = false;
            color->backgroundIntense = false;
        } break;

        case HSML_COLOR_NONE: color->color = SY_COLOR_NONE; break;
        case HSML_COLOR_RED: color->background = SY_COLOR_RED; break;
        case HSML_COLOR_GREEN: color->background = SY_COLOR_GREEN; break;
        case HSML_COLOR_YELLOW: color->background = SY_COLOR_YELLOW; break;
        case HSML_COLOR_BLUE: color->background = SY_COLOR_BLUE; break;
        case HSML_COLOR_PURPLE: color->background = SY_COLOR_PURPLE; break;
        case HSML_COLOR_CYAN: color->background = SY_COLOR_CYAN; break;
        case HSML_COLOR_WHITE: color->background = SY_COLOR_WHITE; break;

        case HSML_COLOR_INTENSE: color->backgroundIntense = true; break;
        case HSML_COLOR_FAINT: color->backgroundFaint = true; break;

        case HSML_COLOR_NOT_INTENSE: color->backgroundIntense = false; break;
        case HSML_COLOR_NOT_FAINT: color->backgroundFaint = false; break;
        default:
            return;
    }
}

static void get_text_color_token(ui_state* state, file_descriptor descriptor, bool isBackground)
{
    text_color* color = &state->color;
    char symbol;
    char null = 0;

    u32 currentWord = 0;
    fixed_array buffer[SCA_HSML_MAX_COLOR_KEYWORDS];
    for (u8 i = 0; i < SCA_HSML_MAX_COLOR_KEYWORDS; ++i)
        fixed_array_init(&buffer[i], 32 * sizeof(char));

    while(true)
    {
        platform_read_file(descriptor, &symbol, 1);

        if (symbol == ' ' || symbol == '\n' || symbol == '\r')
            continue;

        if (symbol == ';')
        {
            fixed_array_push(&buffer[currentWord], &null, 1);
            break;
        }

        if (symbol == ',')
        {
            fixed_array_push(&buffer[currentWord], &null, 1);
            currentWord++;

            if (currentWord > SCA_HSML_MAX_COLOR_KEYWORDS)
                log_critical_s("Invalid HSML: too many color keywords.", 39);

            continue;
        }

        fixed_array_push(&buffer[currentWord], &symbol, 1);
    }

    for (u8 i = 0; i < SCA_HSML_MAX_COLOR_KEYWORDS; ++i)
    {
        hsml_color_token_type colorToken = get_color_token(&buffer[i]);
        if (colorToken == HSML_COLOR_INVALID)
            continue;

        if (!isBackground)
            handle_color_token(colorToken, color);
        else
            handle_bg_color_token(colorToken, color);
    }

    for (u8 i = 0; i < SCA_HSML_MAX_COLOR_KEYWORDS; ++i)
        fixed_array_destroy(&buffer[i]);
}

static void handle_if_numeric(ui_state* state, file_descriptor descriptor, bool* shallSkip)
{
    *shallSkip = get_numeric_token(descriptor) <= 0;
}

static void handle_if(ui_state* state, file_descriptor descriptor, bool* shallSkip)
{
    memory_pool* pool = state->pool;
    u8 possibleStackParameterAmount = (u8)pool[SCA_STACK_ADDRESS + pool[SCA_STACK_SIZE_ADDRESS] - 1];

    // Get parameter:
    char first = 0;
    while(true)
    {
        platform_read_file(descriptor, &first, 1);
        if (first != ' ')
            break;
    }

    if (first != '%')
    {
        log_critical_s("Invalid HSML: invalid if condition", 35);
        return;
    }
    
    u8 index = 0;
    bool numberFound = false;
    while(true)
    {
        char next = 0;  
        platform_read_file(descriptor, &next, 1);
    
        if (!isdigit(next))
            break;

        numberFound = true;
        index += next - '0';
    }

    if (!numberFound || index >= possibleStackParameterAmount)
    {
        log_critical_s("Invalid HSML: invalid parameter index", 35);
        return;
    }

    u8 value = pool[SCA_STACK_ADDRESS + pool[SCA_STACK_SIZE_ADDRESS] - possibleStackParameterAmount - 1 + index];

    // Get operator:
    enum operator : u8
    {
        INVALID = 0,
        EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS_EQUAL,
        LESS
    };

    enum operator operator = INVALID;
    while(true)
    {
        char next = 0;  
        platform_read_file(descriptor, &next, 1);

        if (next == ' ')
            continue;

        if (next == '=')
        {
            char follow = 0;  
            platform_read_file(descriptor, &follow, 1);
            if (follow == '=')
            {
                operator = EQUAL;
                break;
            }
            
            log_critical_s("Invalid HSML: invalid EQUAL operator", 35);
        }

        if (next == '>')
        {
            char follow = 0;  
            platform_read_file(descriptor, &follow, 1);
            if (follow == '=')
            {
                operator = GREATER_EQUAL;
                break;
            }

            operator = GREATER;
            break;
        }

        if (next == '<')
        {
            char follow = 0;  
            bool valid = platform_read_file(descriptor, &follow, 1);
            if (valid && follow == '=')
            {
                operator = LESS_EQUAL;
                break;
            }

            operator = LESS;
            break;
        }
    }

    // Get number:
    u32 compareNumber = get_numeric_token(descriptor);

    if (operator == EQUAL)
    {
        *shallSkip = !(value == compareNumber);
        return;
    }

    if (operator == GREATER)
    {
        *shallSkip = !(value > compareNumber);
        return;
    }
    if (operator == GREATER_EQUAL)
    {
        *shallSkip = !(value >= compareNumber);
        return;
    }

    if (operator == LESS)
    {
        *shallSkip = !(value < compareNumber);
        return;
    }
    if (operator == LESS_EQUAL)
    {
        *shallSkip = !(value <= compareNumber);
        return;
    }

   

    log_critical_s("Invalid HSML: no if operator", 29);
}

static void handle_token(ui_state* state, file_descriptor descriptor, enum hsml_mode* mode, u32* helperValue, bool* shallSkip)
{
    hsml_token currentToken = get_next_token(descriptor);

    if (*shallSkip && currentToken == HSML_TOKEN_END_IF)
    {
        *shallSkip = false;
        return;
    }

    if (*shallSkip)
        return;

    switch(currentToken)
    {
        case HSML_TOKEN_NONE:
            *mode = HSML_MODE_NONE;
            break;

        case HSML_TOKEN_INCLUDE:
        {
            dynamic_array buffer = { 0 };
            get_text_token(descriptor, &buffer);
            ui_hsml(state, buffer.buffer);
            dynamic_array_destroy(&buffer);
        } break;

        case HSML_TOKEN_IF:
        {
            handle_if(state, descriptor, shallSkip);
            break;
        }
        case HSML_TOKEN_IFN:
        {
            handle_if_numeric(state, descriptor, shallSkip);
            break;
        }
        case HSML_TOKEN_END_IF:
            break;

        case HSML_TOKEN_START:
        {
            *helperValue = get_numeric_token(descriptor);
            *mode = HSML_MODE_TEXT; 
        } break;
        case HSML_TOKEN_END:
            *mode = HSML_MODE_NONE; 
            break;

        case HSML_TOKEN_ALIGN_CENTER:
        {
            u32 xOffset = get_numeric_token(descriptor);
            ui_set_align(state, UI_ALIGN_CENTER, xOffset);
        } break;
        case HSML_TOKEN_ALIGN_LEFT:
        {
            u32 xOffset = get_numeric_token(descriptor);
            ui_set_align(state, UI_ALIGN_LEFT, xOffset);
        } break;
        case HSML_TOKEN_ALIGN_RIGHT:
        {
            u32 xOffset = get_numeric_token(descriptor);
            ui_set_align(state, UI_ALIGN_RIGHT, xOffset);
        } break;

        case HSML_TOKEN_POS_NONE:
        {
            u32 yOffset = get_numeric_token(descriptor);
            ui_set_position(state, UI_POS_NONE, yOffset);
        } break;
        case HSML_TOKEN_POS_BOTTOM:
        {
            u32 yOffset = get_numeric_token(descriptor);
            ui_set_position(state, UI_POS_BOTTOM, yOffset);
        } break;
        case HSML_TOKEN_POS_TOP:
        {
            u32 yOffset = get_numeric_token(descriptor);
            ui_set_position(state, UI_POS_TOP, yOffset);
        } break;

        case HSML_TOKEN_HLINE:
        {
            u32 yOffset = get_numeric_token(descriptor);
            char character = get_character_token(descriptor);
            ui_hline(state, yOffset, character);
        } break;
        case HSML_TOKEN_SPACE:
        {
            u32 amount = get_numeric_token(descriptor);
            ui_space(state, amount);
        } break;
        case HSML_TOKEN_NUDGE:
        {
            u32 amount = get_numeric_token(descriptor);
            ui_nudge(state, amount);
        } break;
        case HSML_TOKEN_FEED:
        {
            ui_feed(state);
        } break;
        case HSML_TOKEN_SAMELINE:
        {
            u32 sameline = get_numeric_token(descriptor);
            ui_sameline(state, sameline > 0);
        } break;

        case HSML_TOKEN_COLOR:
        {
            get_text_color_token(state, descriptor, false);
            ui_set_color(state, &state->color);
        } break;
        case HSML_TOKEN_BG_COLOR:
        {
            get_text_color_token(state, descriptor, true);
            ui_set_color(state, &state->color);
        } break;
        case HSML_TOKEN_BG:
        {
            u32 renderBG = get_numeric_token(descriptor);
            state->color.renderBackground = renderBG > 0;
            ui_set_color(state, &state->color);
        } break;
    }
}

static void handle_text(ui_state* state, file_descriptor descriptor, enum hsml_mode* mode, u32 helperValue, char current)
{
    memory_pool* pool = state->pool;
    u8 possibleStackParameterAmount = (u8)pool[SCA_STACK_ADDRESS + pool[SCA_STACK_SIZE_ADDRESS] - 1];

    fixed_array buffer = { 0 };
    fixed_array_init(&buffer, helperValue);
    fixed_array_push(&buffer, &current, 1);
    if (helperValue - 1 > 0)
        platform_read_file(descriptor, buffer.buffer + 1, helperValue - 1);
    buffer.current += helperValue - 1;

    // Check for placeholders:
    dynamic_array finalText = { 0 };
    dynamic_array_init(&finalText, buffer.capacity, sizeof(char));
    {
        char* data = (char*)buffer.buffer;
        for (u32 i = 0; i < buffer.current; ++i)
        {
            char current = data[i];
            if (current != '%')
            {
                dynamic_array_push(&finalText, &current, 1);
                continue;
            }

            u8 index = 0;
            bool numberFound = false;
            while(true)
            {
                if (i + 1 >= buffer.current)
                    break;

                char next = data[i + 1];                  
                if (!isdigit(next))
                    break;

                numberFound = true;
                index += next - '0';
                ++i;
            }

            if (!numberFound || index >= possibleStackParameterAmount)
            {
                char percentageSymbol = '%';
                dynamic_array_push(&finalText, &percentageSymbol, 1);
                continue;
            }

            u8 value = pool[SCA_STACK_ADDRESS + pool[SCA_STACK_SIZE_ADDRESS] - possibleStackParameterAmount - 1 + index];
            if (value == 0)
            {
                char zero = '0';
                dynamic_array_push(&finalText, &zero, 1);
                continue;
            }

            char temp[SCA_HSML_MAX_TEXT_VALUE_LENGTH];
            u32 length = 0;
            while (value > 0)
            {
                temp[length] = (value % 10) + '0';
                value /= 10;
                ++length;
            }

            for (i32 i = length - 1; i >= 0; --i)
                dynamic_array_push(&finalText, &temp[i], 1);
        }
    }

    ui_text(state, finalText.buffer, helperValue);

    dynamic_array_destroy(&finalText);
    fixed_array_destroy(&buffer);

    helperValue = 0;
    *mode = HSML_MODE_NONE;
    current = 0;
}

void ui_hsml(ui_state* state, const char* filepath)
{
    file_descriptor descriptor = platform_open_file(filepath, SCA_FILE_READ);
    if (descriptor == invalid_file_descriptor)
    {
        log_critical_s("Invalid filepath", 17);
        return;
    }

    enum hsml_mode mode = HSML_MODE_NONE;
    u32 helperValue = 0;

    bool shallSkip = false;
    char current = 0;
    while(platform_read_file(descriptor, &current, 1))
    {
        if (mode == HSML_MODE_COMMENT)
        {
            if (current == '\n')
                mode = HSML_MODE_NONE;
            continue;
        }

        if (current == ' ' || current == '\n' || current == '\r' || current == '\0')
            continue;

        if (current == 0 || current == EOF)
            break;

        if (current == '#')
        {
            mode = HSML_MODE_COMMENT;
            continue;
        }

        if (current == '\\')
        {
            mode = HSML_MODE_FORMAT;
            handle_token(state, descriptor, &mode, &helperValue, &shallSkip);
            continue;
        }

        if (mode == HSML_MODE_TEXT)
            handle_text(state, descriptor, &mode, helperValue, current);
    }

    platform_close_file(descriptor);
}