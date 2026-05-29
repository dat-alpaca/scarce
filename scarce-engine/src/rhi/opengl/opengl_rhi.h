#pragma once
#include "core/defines.h"

typedef enum : u32
{
    gl_invalid = 0xFFFFFFFF
} gl_handle;

typedef struct opengl_rhi
{
    gl_handle vao;
    void* platformData;
} opengl_rhi;