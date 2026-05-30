#pragma once
#include "defines.h"

typedef enum : u8
{
    TAG_LINEAR_START = 0,
    TAG_UNKNOWN = TAG_LINEAR_START,
    TAG_SYSTEM,
    TAG_USER,
    TAG_LINEAR_END,

    TAG_GENERAL_START,
    TAG_ASSETS = TAG_GENERAL_START,
    TAG_RENDERER,
    TAG_GENERAL,
    TAG_GENERAL_END,
    
    TAG_TRANSIENT,      // uses the stack arena
    TAG_AMOUNT,
} memory_tag;

static inline bool is_linear_memory_tag(memory_tag tag)
{
    return tag == TAG_UNKNOWN || tag == TAG_SYSTEM || tag == TAG_USER;
}

static inline const char* get_memory_tag_name(memory_tag tag)
{
    switch(tag)
    {
        case TAG_UNKNOWN: return "UNKNOWN";
        case TAG_SYSTEM: return "SYSTEM";
        case TAG_USER: return "USER";
        case TAG_ASSETS: return "ASSETS";
        case TAG_RENDERER: return "RENDERER";
        case TAG_GENERAL: return "GENERAL";
        case TAG_TRANSIENT: return "TRANSIENT";
        case TAG_AMOUNT: return "AMOUNT";
        default: return "invalid";
    }

    return "none";
}