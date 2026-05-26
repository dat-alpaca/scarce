#pragma once
#include "core/defines.h"

typedef enum
{
    SHADER_VERTEX,
    SHADER_FRAGMENT
} shader_type;

typedef struct
{
    u32 binding;
    u32 location;
    u32 format;
    u32 formatSize;
    u32 offset;
} attribute;

typedef struct
{
    u32 binding;
    u32 stride;
} binding;

typedef struct
{
    binding* bindings;
    attribute* attributes;

    u32 attributeSize;
    u32 bindingSize;
} input_stage;

typedef enum : u32 
{  
    pipeline_invalid = 0xFFFFFFFF
} pipeline_handle;

typedef struct
{
    pipeline_handle handle;
    input_stage inputStage;
} pipeline;