#pragma once
#include "defines.h"

#define SCA_LINEAR_TAGS (TAG_AMOUNT - 1)

typedef enum : u8
{
    TAG_UNKNOWN = 0,
    TAG_SYSTEM,
    TAG_USER,
    TAG_GENERAL,    
    
    
    TAG_TRANSIENT,
    TAG_AMOUNT,
} memory_tag;