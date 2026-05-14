#include "hsml.h"

#include "dynamic_array.h"
#include "logging/logger.h"

#include "memory/memory.h"
#include "ui/button.h"
#include "ui/ui.h"
#include "token.h"
#include "defines.h"

#include <string.h>

#define SCA_HSML_MAX_TEXT_VALUE_LENGTH 3

static void hsml_parse_argless_token(ui_state* state, hsml_token_type type, hsml_mode* mode, bool* executing)
{
    switch(type)
    {
        case HSML_TOKEN_END_IF:
            *executing = true;
            break;

        case HSML_TOKEN_START:
            *mode = HSML_MODE_TEXT; 
            break;
        case HSML_TOKEN_END:
            *mode = HSML_MODE_NORMAL; 
            break;

        case HSML_TOKEN_FEED:
            ui_feed(state);    
            break;

        case HSML_TOKEN_FEEDLINE:
        {
            ui_feed(state);    
            ui_space(state, 1);
        } break;

        default:
            log_critical_s("Invalid HSML: invalid argless token parsed", 45);
    }
}

static void hsml_parse_numeric_token(ui_state* state, hsml_token_type type, u32 number, bool* executing)
{
    switch(type)
    {
        case HSML_TOKEN_IF:
            *executing = (number > 0);
            break;

        case HSML_TOKEN_SAMELINE:
            state->sameLine = (number > 0);
            break;
        
        case HSML_TOKEN_ALIGN_CENTER:
            ui_set_align(state, UI_ALIGN_CENTER, number);
            break;
        case HSML_TOKEN_ALIGN_LEFT:
            ui_set_align(state, UI_ALIGN_LEFT, number);
            break;
        case HSML_TOKEN_ALIGN_RIGHT:
            ui_set_align(state, UI_ALIGN_RIGHT, number);
            break;

        case HSML_TOKEN_POS_NONE:
            ui_set_position(state, UI_POS_NONE, number);
            break;
        case HSML_TOKEN_POS_TOP:
            ui_set_position(state, UI_POS_TOP, number);
            break;
        case HSML_TOKEN_POS_BOTTOM:
            ui_set_position(state, UI_POS_BOTTOM, number);
            break;

        case HSML_TOKEN_SPACE:
            ui_space(state, number);
            break;
        case HSML_TOKEN_NUDGE:
            ui_nudge(state, number);
            break;

        case HSML_TOKEN_BG:
        {
            state->color.renderBackground = (number > 0);
            ui_set_color(state, &state->color);
        } break;

        default:
            log_critical_s("Invalid HSML: invalid numerical token parsed", 45);
    }
    
}

static void hsml_parse_text_token(ui_state* state, hsml_token* token)
{
    switch(token->type)
    {
        case HSML_TOKEN_INCLUDE:
            ui_hsml(state, token->value.buffer);
            break;

        default:
            log_critical_s("Invalid HSML: invalid numerical token parsed", 45);
    }
}

static void hsml_parse_color_token(symbol_color* color, u8* intense, u8* faint, hsml_color_token_type colorToken)
{
    switch(colorToken)
    {
        case HSML_COLOR_DEFAULT:
        {
            *color = SY_COLOR_WHITE;
            *intense = false;
            *faint = false;
        } break;
            
        case HSML_COLOR_NONE: *color = SY_COLOR_NONE; break;
        case HSML_COLOR_RED: *color = SY_COLOR_RED; break;
        case HSML_COLOR_GREEN: *color = SY_COLOR_GREEN; break;
        case HSML_COLOR_YELLOW: *color = SY_COLOR_YELLOW; break;
        case HSML_COLOR_BLUE: *color = SY_COLOR_BLUE; break;
        case HSML_COLOR_PURPLE: *color = SY_COLOR_PURPLE; break;
        case HSML_COLOR_CYAN: *color = SY_COLOR_CYAN; break;
        case HSML_COLOR_WHITE: *color = SY_COLOR_WHITE; break;

        case HSML_COLOR_INTENSE: *intense = true; break;
        case HSML_COLOR_NOT_INTENSE: *intense = false; break;

        case HSML_COLOR_FAINT: *faint = true; break;
        case HSML_COLOR_NOT_FAINT: *faint = false; break;

        default:
            log_critical_s("Invalid HSML: invalid color", 28);
            break;
    }
}

static void hsml_parse_color_tokens(ui_state* state, hsml_token* tokens)
{
    switch(tokens->type)
    {
        case HSML_TOKEN_COLOR:
        {
            hsml_color_token_type* data = (hsml_color_token_type*)tokens->value.buffer; 
            for (u32 i = 0; i < dynamic_array_size(&tokens->value); ++i)
            {
                symbol_color color = state->color.color;
                u8 colorFaint = state->color.colorFaint;
                u8 colorIntense = state->color.colorIntense;

                hsml_color_token_type current = data[i];
                hsml_parse_color_token(
                    &color, 
                    &colorIntense, 
                    &colorFaint, 
                    current
                );

                state->color.color = color;
                state->color.colorFaint = colorFaint;
                state->color.colorIntense = colorIntense;
                ui_set_color(state, &state->color);
            }
        } break;

        case HSML_TOKEN_BG_COLOR:
        {
            hsml_color_token_type* data = (hsml_color_token_type*)tokens->value.buffer; 
            for (u32 i = 0; i < tokens->value.current; ++i)
            {
                symbol_color color = state->color.background;
                u8 colorFaint = state->color.backgroundFaint;
                u8 colorIntense = state->color.backgroundIntense;

                hsml_color_token_type* current = &data[i];
                hsml_parse_color_token(
                    &color, 
                    &colorIntense, 
                    &colorFaint, 
                    *current
                );

                state->color.background = color;
                state->color.backgroundFaint = colorFaint;
                state->color.backgroundIntense = colorIntense;
                ui_set_color(state, &state->color);
            }
        } break;

        default:
            log_critical_s("Invalid HSML: invalid color token parsed", 45);
    }
}

static void hsml_parse_multiple_token(ui_state* state, hsml_token* token)
{
    memory_pool* pool = state->pool;

    switch(token->type)
    {
        case HSML_TOKEN_HLINE:
        {
            char* character = (char*)(token->value.buffer);
            u32* yOffset = (u32*)(token->value.buffer + sizeof(u32));
            ui_hline(state, *yOffset, (char)*character);
        } break;

        case HSML_TOKEN_BUTTON:
        {
            u8 baseAddressIndex = *(u8*)(token->value.buffer);
            u8 buttonIndex = *(u8*)(token->value.buffer + sizeof(u8));
            char* contents = (char*)(token->value.buffer + sizeof(u8) * 2);

            ui_button* buttons = (ui_button*)&pool[baseAddressIndex];
            ui_button* button = &buttons[buttonIndex];
            
            ui_button_render(button, state, contents);
        } break;
    
        default:
            log_critical_s("Invalid HSML: invalid multiple token parsed", 45);
    }
}

static void hsml_parse_tokens(ui_state* state, hsml_token* token, hsml_mode* mode, bool* executing)
{
    bool tokenProcessed = true;
    hsml_token_argument argumentType = hsml_get_argument_type(token->type); 
    switch(argumentType)
    {
        case HSML_TOKEN_ARG_NONE:
            hsml_parse_argless_token(state, token->type, mode, executing);
            break;

        case HSML_TOKEN_ARG_NUMERIC:
        {
            u32* value = (u32*)token->value.buffer;
            hsml_parse_numeric_token(state, token->type, *value, executing);
        } break;

        case HSML_TOKEN_ARG_TEXT:        
            hsml_parse_text_token(state, token);
            break;

        case HSML_TOKEN_ARG_COLOR:
            hsml_parse_color_tokens(state, token);
            break;

        case HSML_TOKEN_ARG_MULTIPLE:
            hsml_parse_multiple_token(state, token);
            break;

        default:
            tokenProcessed = false;
            break;
    }

    if (tokenProcessed && token->value.buffer)
        dynamic_array_destroy(&token->value);
}

static void hsml_parse_text(ui_state* state, hsml_token* token, hsml_token* next)
{
    ui_text(state, token->value.buffer, token->value.current);
    
    if (next->type == HSML_TOKEN_TEXT)
        ui_feed(state);

    if (token->value.buffer)
        dynamic_array_destroy(&token->value);
}

void ui_hsml(ui_state* state, const char* filepath)
{
    dynamic_array tokens = { 0 };
    hsml_tokenize(state, filepath, &tokens);

    hsml_token* next = NULL;
    bool executing = true;
    hsml_mode mode = HSML_MODE_NORMAL;
    for (u32 i = 0; i < dynamic_array_size(&tokens); ++i)
    {
        hsml_token* data = (hsml_token*)tokens.buffer;
        hsml_token* current = &data[i];
        if (i < dynamic_array_size(&tokens) - 1)
            next = &data[i + 1];

        if (!executing && current->type != HSML_TOKEN_END_IF)
            continue;

        hsml_parse_tokens(state, current, &mode, &executing);

        if (mode == HSML_MODE_TEXT && current->type == HSML_TOKEN_TEXT)
            hsml_parse_text(state, current, next);
    }

    dynamic_array_destroy(&tokens);
}