#pragma once
#include "ui/ui.h"
#include "token.h"

enum 
{ 
    HSML_PLACEHOLDER_READ_U8,
    HSML_PLACEHOLDER_READ_U16,
    HSML_PLACEHOLDER_READ_U32,
    HSML_PLACEHOLDER_READ_U64,
    HSML_PLACEHOLDER_READ_STRING,
} typedef hsml_placeholder_read_mode; 

hsml_token hsml_fetch_placeholder_value(ui_state* state, file_descriptor descriptor);

u64 hsml_fetch_number_from_placeholder(hsml_token* token);