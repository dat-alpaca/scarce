#pragma once
#include "platform/platform.h"
#include "ui/ui.h"

typedef enum : u8
{
    HSML_OPERATOR_INVALID,
    HSML_OPERATOR_NUMBER,
    
    HSML_OPERATOR_MUL,      // *
    HSML_OPERATOR_DIV,      // /
    HSML_OPERATOR_MOD,      // %
    HSML_OPERATOR_PLS,      // +
    HSML_OPERATOR_MNS,      // -
    
    HSML_OPERATOR_EQU,      // ==
    HSML_OPERATOR_NEQ,      // !=
    HSML_OPERATOR_GRT,      // >
    HSML_OPERATOR_LSS,      // <
    HSML_OPERATOR_GEQ,      // >=
    HSML_OPERATOR_LEQ,      // <=
    
    HSML_OPERATOR_AND,      // &&
    HSML_OPERATOR_OR,       // ||

    HSML_OPERATOR_OPEN,     // (
    HSML_OPERATOR_CLOSE,    // )
} hsml_operator_type;

typedef struct
{
    hsml_operator_type type;
    u32 value;
} hsml_evaluation;

i32 hsml_get_expression_evaluation(ui_state* state, file_descriptor descriptor);