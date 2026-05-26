#pragma once
#include "core/defines.h"

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

typedef enum : u32 
{  
    texture_invalid = 0xFFFFFFFF
} texture_handle;