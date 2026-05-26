#pragma once
#include "dynamic_array.h"
#include "assets/spritesheet.h"
#include "fixed_array.h"
#include <freetype/freetype.h>

#define SCA_AUTO_CHANNELS (0)

typedef u32 asset_handle;

typedef struct asset_library
{
    dynamic_array spritesheets;
    FT_Library fontLibrary;
} asset_library;

void asset_library_init(asset_library* library, u32 spritesheetCapacity);

asset_handle asset_library_load_spritesheet(asset_library* library, const char* filepath, u32 spriteSize);
asset_handle asset_library_load_sprites(asset_library* library, fixed_array* filepaths, u32 spriteSize, u32 channels);
asset_handle asset_library_load_font(asset_library* library, const char* filepath, u32 height);

spritesheet* asset_library_get_spritesheet(asset_library* library, asset_handle handle);
