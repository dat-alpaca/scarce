#pragma once
#include <cglm/cglm.h>

#include "core/defines.h"
#include "graphics/buffer.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "graphics/pipeline.h"

#include "platform/platform.h"

typedef void* rhi;
rhi rhi_init();

void rhi_begin_frame(rhi rhi);
void rhi_end_frame(rhi rhi);
void rhi_set_viewport(rhi rhi, i32 x, i32 y, i32 width, i32 height);

// Buffers:
buffer_handle rhi_create_buffer(rhi rhi, u32 size, buffer_usage usage);
void rhi_update_buffer(rhi rhi, buffer_handle bufferHandle, void* buffer, u32 size, u32 offset);
void rhi_bind_buffer(rhi rhi, buffer_handle bufferHandle, u32 binding, u32 stride); 
void rhi_bind_element_buffer(rhi rhi, buffer_handle bufferHandle); 
void rhi_destroy_buffer(rhi rhi, buffer_handle bufferHandle);

// Textures:
texture_handle rhi_create_texture(rhi rhi, texture_information* information);
void rhi_update_texture(rhi rhi, texture_handle texture, texture_information* information, void* data, u32 dataWidth, u32 dataHeight, u32 dataFormat, u32 dataFormatSize, u32 dataType);
void rhi_update_texture_array_layer(rhi rhi, texture_handle texture, texture_information* information, u32 layer, vec2 offset, u32 dataFormat, u32 dataType, void* data);
void rhi_destroy_texture(rhi rhi, texture_handle texture);

// Pipelines:
shader_handle rhi_create_shader(rhi rhi, shader_type type, char* shaderContents);
pipeline rhi_create_pipeline(rhi rhi, shader_handle vertexShader, shader_handle fragmentShader);
void rhi_bind_pipeline(rhi rhi, pipeline pipeline);

void rhi_bind_ssbo(rhi rhi, buffer_handle buffer, u32 binding);
void rhi_bind_ubo(rhi rhi, buffer_handle buffer, u32 binding);
void rhi_bind_texture(rhi rhi, texture_handle texture, u32 binding);

// Draw calls:
void rhi_draw(rhi rhi, u32 vertexCount);

// Platform-dependent:
void rhi_initialize_window(rhi rhi, window_handle window);

void rhi_swap_buffers(rhi rhi, window_handle window);