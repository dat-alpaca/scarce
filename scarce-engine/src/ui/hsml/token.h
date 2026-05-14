#pragma once
#include "dynamic_array.h"
#include "ui/ui.h"

typedef enum
{
    HSML_MODE_NORMAL,
    HSML_MODE_TEXT,
} hsml_mode;

typedef enum
{
    HSML_TOKEN_INVALID = 0,
    HSML_TOKEN_NUMERIC,
    HSML_TOKEN_TEXT,
    
    HSML_TOKEN_START,
    HSML_TOKEN_END,

    HSML_TOKEN_IF,
    HSML_TOKEN_END_IF,

    HSML_TOKEN_INCLUDE,
    HSML_TOKEN_BUTTON,
    HSML_TOKEN_TEXTBOX,

    HSML_TOKEN_ALIGN_CENTER,
    HSML_TOKEN_ALIGN_LEFT,
    HSML_TOKEN_ALIGN_RIGHT,

    HSML_TOKEN_POS_TOP,
    HSML_TOKEN_POS_BOTTOM,
    HSML_TOKEN_POS_NONE,

    HSML_TOKEN_HLINE,
    HSML_TOKEN_SPACE,
    HSML_TOKEN_NUDGE,
    HSML_TOKEN_FEED,
    HSML_TOKEN_FEEDLINE,

    HSML_TOKEN_SAMELINE,
    HSML_TOKEN_COLOR,
    HSML_TOKEN_BG_COLOR,
    HSML_TOKEN_BG,
} hsml_token_type;

typedef struct hsml_token_lookup_item
{
    const char* name;
    hsml_token_type type;
} hsml_token_lookup_item;

typedef enum : u8
{
    HSML_COLOR_INVALID = 0,
    
    HSML_COLOR_DEFAULT,
    HSML_COLOR_NONE,
    HSML_COLOR_RED,
    HSML_COLOR_GREEN,
    HSML_COLOR_YELLOW,
    HSML_COLOR_BLUE,
    HSML_COLOR_PURPLE,
    HSML_COLOR_CYAN,
    HSML_COLOR_WHITE,
    
    HSML_COLOR_INTENSE,
    HSML_COLOR_NOT_INTENSE,
    
    HSML_COLOR_FAINT,
    HSML_COLOR_NOT_FAINT,
} hsml_color_token_type;

typedef struct hsml_token_color_item
{
    const char* name;
    hsml_color_token_type color;
} hsml_token_color_item;

typedef enum
{
    HSML_TOKEN_ARG_INVALID,
    HSML_TOKEN_ARG_NONE,
    HSML_TOKEN_ARG_NUMERIC,
    HSML_TOKEN_ARG_TEXT,
    HSML_TOKEN_ARG_COLOR,
    HSML_TOKEN_ARG_MULTIPLE,
} hsml_token_argument;

typedef struct
{
    hsml_token_type type;
    dynamic_array value;
} hsml_token;

hsml_token_argument hsml_get_argument_type(hsml_token_type type);

void hsml_tokenize(ui_state* state, const char* filepath, dynamic_array* tokens);