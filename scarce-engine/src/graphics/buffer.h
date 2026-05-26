#pragma once
#include "core/defines.h"

typedef enum : u8
{
    BUFFER_DYNAMIC,
    BUFFER_READ_ONLY,
    BUFFER_WRITE_ONLY,
    BUFFER_PERSISTENT,
    BUFFER_COHERENT,
} buffer_usage;

typedef enum : u32 
{  
    buffer_invalid = 0xFFFFFFFF
} buffer_handle;