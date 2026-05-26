#pragma once
#include "core/defines.h"

typedef struct shader_filepaths
{
    const char* vertexFilepath;
    const char* fragmentFilepath;
} shader_filepaths;

typedef enum : u32 
{  
    shader_invalid = 0xFFFFFFFF
} shader_handle;