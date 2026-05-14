#pragma once
#include "platform/platform.h"
#include "ui/ui.h"

typedef enum : u8
{
    HSML_COND_INVALID,
    HSML_COND_NUMBER,
    
    HSML_COND_EQU,      // ==
    HSML_COND_NEQ,      // !=
    HSML_COND_GRT,      // >
    HSML_COND_LSS,      // <
    HSML_COND_GEQ,      // >=
    HSML_COND_LEQ,      // <=
    
    HSML_COND_AND,      // &&
    HSML_COND_OR,       // ||

    HSML_COND_OPEN,     // (
    HSML_COND_CLOSE,    // )
} hsml_conditional_type;

typedef struct
{
    hsml_conditional_type type;
    u32 value;
} hsml_conditional;

bool hsml_get_conditional_result(ui_state* state, file_descriptor descriptor);