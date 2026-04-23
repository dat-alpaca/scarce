#pragma once
#include "core/defines.h"
#include <cglm/cglm.h>

typedef u32 gl_handle;

void graphics_begin_frame(gl_handle vao);
void graphics_end_frame();

// Buffers:
typedef enum
{
    BUFFER_DYNAMIC,
    BUFFER_READ_ONLY,
    BUFFER_WRITE_ONLY,
    BUFFER_PERSISTENT,
    BUFFER_COHERENT,
} buffer_usage;

gl_handle graphics_create_buffer(u32 size, buffer_usage usage);
void graphics_update_buffer(gl_handle bufferHandle, void* buffer, u32 size, u32 offset);
void graphics_bind_buffer(gl_handle vao, gl_handle bufferHandle, u32 binding, u32 stride); 
void graphics_bind_element_buffer(gl_handle vao, gl_handle bufferHandle); 
void graphics_destroy_buffer(gl_handle bufferHandle);


// Textures:
typedef struct
{
    u32 width;
	u32 height;
	u32 depth;
	u32 mipLevels;
	u32 samples;
	u32 arrayLayers;
	u32 layerWidth;
	u32 layerHeight;


    u32 format;
    u32 type;


    u32 wrapS;
    u32 wrapT;
    u32 minFilter;
    u32 magFilter;

	bool generateMipmaps;
} texture_information;

gl_handle graphics_create_texture(texture_information* information);
void graphics_update_texture(gl_handle texture, texture_information* information, void* data, u32 dataWidth, u32 dataHeight, u32 dataFormat, u32 dataFormatSize, u32 dataType);
void graphics_update_texture_array_layer(gl_handle texture, texture_information* information, u32 layer, vec2 offset, u32 dataFormat, u32 dataType, void* data);
void graphics_destroy_texture(gl_handle texture);

void graphics_draw(u32 vertexCount);