#pragma once
#include "core/defines.h"

typedef u32 gl_handle;

typedef struct opengl_rhi
{
    void* platformData;
    gl_handle vao;
} opengl_rhi;