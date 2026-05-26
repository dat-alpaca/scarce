#pragma once
#include "fixed_array.h"

typedef struct sprite
{
	fixed_array data;
	u32 layer;
} sprite;

typedef struct spritesheet
{
	fixed_array sprites;
	u32 spriteSize;
	u8 channels;
} spritesheet;