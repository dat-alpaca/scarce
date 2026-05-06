#pragma once
#include <stdbool.h>
#include <cglm/cglm.h>
#include <ft2build.h>
#include <freetype/freetype.h>

#include "core/defines.h"
#include "core/fixed_array.h"

#define SCA_FONT_START_INDEX (0)
#define SCA_FONT_END_INDEX ('~')

typedef struct
{
	fixed_array data;
	vec2 size;
	vec2 bearing;
	i64 advance;
	u32 layer;
	char letter;
} font_character;

typedef struct
{
    fixed_array characters;
    FT_Face face;
    
    u32 maxHeight;
    u32 maxWidth;
} font;

bool font_loader_init(FT_Library* library);

bool load_font(FT_Library* library, font* output, const char* filepath);