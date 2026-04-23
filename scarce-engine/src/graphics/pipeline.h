#pragma once
#include "graphics/graphics.h"

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

typedef struct
{
    gl_handle handle;
    input_stage inputStage;
} pipeline;

gl_handle graphics_create_shader(shader_type type, char* shaderContents);
pipeline graphics_create_pipeline(gl_handle vertexShader, gl_handle fragmentShader);
void graphics_bind_pipeline(gl_handle vao, pipeline pipeline);

void graphics_bind_ssbo(gl_handle buffer, u32 binding);
void graphics_bind_ubo(gl_handle buffer, u32 binding);
void graphics_bind_texture(gl_handle texture, u32 binding);