#pragma once
#include <cglm/cglm.h>
#include "buffer.h"
#include "core/fixed_array.h"
#include "graphics/pipeline.h"
#include "platform/platform.h"

#include "graphics/shader.h"
#include "rhi/rhi.h"
#include "texture.h"

typedef struct batch_renderer_cell
{
    vec2 position;
    u32 layer;
    u32 paintBackground;
    vec4 color;
    vec4 backgroundColor;
} batch_renderer_cell;

typedef struct
{
    mat4 model;
    mat4 projection;
} batch_renderer_world;

typedef struct batch_renderer
{
    batch_renderer_world world;
    window_handle* window;
    fixed_array cells;
    rhi rhi;
    
    pipeline pipeline;
    
    u32 vao;
    buffer_handle VBO;
    buffer_handle worldUBO;
    buffer_handle charactersSSBO;
    
    texture_handle spritesheetTexture;
    
    u32 windowWidth;
    u32 windowHeight;
    u32 gridWidth;
    u32 gridHeight;
    u32 scale;
} batch_renderer;

void batch_renderer_init(batch_renderer* renderer, rhi rhi, window_handle window, shader_filepaths* shaders, i32 windowWidth, i32 windowHeight, u32 scale);
void batch_renderer_on_resize(batch_renderer* renderer, i32 windowWidth, i32 windowHeight);
void batch_renderer_set_texture(batch_renderer* renderer, texture_handle texture);
void batch_renderer_set_world(batch_renderer* renderer, mat4 projection);
void batch_renderer_set_scale(batch_renderer* renderer, u32 scale);

void batch_renderer_zero_buffer(batch_renderer* renderer);
void batch_renderer_render(batch_renderer* renderer);

batch_renderer_cell* batch_renderer_get_cell(batch_renderer* renderer, u32 x, u32 y);
void batch_renderer_set_cell(batch_renderer* renderer, batch_renderer_cell* cell, u32 x, u32 y);

void batch_renderer_get_mouse_grid_position(window_handle* window, batch_renderer* renderer, u32* x, u32* y);