#pragma once
#include "graphics/pipeline.h"

typedef struct
{
    const char* vertexFilepath;
    const char* fragmentFilepath;
} shader_filepaths;

gl_handle read_shader(const char* filepath, shader_type type);