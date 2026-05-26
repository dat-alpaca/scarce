#pragma once
#include "core/defines.h"

typedef enum : u8
{
    SCA_2D_TEXTURE,
    SCA_2D_TEXTURE_ARRAY,
} texture_type;

typedef enum : u8
{
    SCA_TEXTURE_REPEAT,
    SCA_TEXTURE_CLAMP_EDGE,
    SCA_TEXTURE_MIRRORED_REPEAT,
} texture_wrap;

typedef enum : u8
{
    SCA_TEXTURE_RED,
    SCA_TEXTURE_RG,
    SCA_TEXTURE_RGB,
    SCA_TEXTURE_RGBA
} texture_pixel_format;

typedef enum : u8
{
    SCA_TEXTURE_R8,
    SCA_TEXTURE_RG8,
    SCA_TEXTURE_RGB8,
    SCA_TEXTURE_RGBA8
} texture_pixel_internal_format;

typedef enum : u8
{
    SCA_TEXTURE_NEAREST,
    SCA_TEXTURE_LINEAR,
    SCA_TEXTURE_NEAREST_MIPMAP_NEAREST,
    SCA_TEXTURE_LINEAR_MIPMAP_NEAREST,
    SCA_TEXTURE_NEAREST_MIPMAP_LINEAR,
    SCA_TEXTURE_LINEAR_MIPMAP_LINEAR,
} texture_filter;

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

    texture_pixel_internal_format internalFormat;
    texture_type type;

    texture_wrap wrapS;
    texture_wrap wrapT;
    texture_filter minFilter;
    texture_filter magFilter;

	bool generateMipmaps;
} texture_information;

typedef enum : u32 
{  
    texture_invalid = 0xFFFFFFFF
} texture_handle;



