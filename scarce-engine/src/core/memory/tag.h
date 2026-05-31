#pragma once
#include "defines.h"

typedef u32 memory_tag;

typedef enum : u8
{
    TAG_START           = 0x0,
    
    TAG_UNKNOWN         = TAG_START,
    TAG_SYSTEM          = 0x1,
    TAG_USER            = 0x2,
    
    TAG_LINEAR_AMOUNT   = 3
} linaer_memory_tag;

typedef enum : u8
{
    TAG_GENERAL_START   = 0x3,
    
    TAG_ASSETS          = TAG_GENERAL_START,
    TAG_RENDERER        = 0x4,
    TAG_GENERAL         = 0x5,
    TAG_HSML            = 0x6,

    TAG_GENERAL_AMOUNT  = 4
} general_memory_tag;

typedef enum : u8
{
    TAG_TRANSIENT       = 0x7,
    TAG_AMOUNT = 8
} system_memory_tag;

static inline bool is_linear_system_memory_tag(memory_tag tag)
{
    return tag == TAG_UNKNOWN || tag == TAG_SYSTEM || tag == TAG_USER;
}

static inline const char* get_system_memory_tag_name(memory_tag tag)
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
        case TAG_HSML: return "HSML";
        case TAG_AMOUNT: return "AMOUNT";
        default: return "invalid";
    }

    return "none";
}