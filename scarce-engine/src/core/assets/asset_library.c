#include <assert.h>
#include <stdint.h>
#include <stb_image.h>

#include "asset_library.h"
#include "defines.h"
#include "dynamic_array.h"
#include "assets/spritesheet.h"
#include "fixed_array.h"
#include "logging/logger.h"
#include "memory/tag.h"

#define SCA_FONT_MAX_INDEX UINT8_MAX

void asset_library_init(asset_library* library, u32 spritesheetCapacity)
{
    assert(library);
    assert(!library->fontLibrary);
    assert(spritesheetCapacity > 0);

    FT_Init_FreeType(&library->fontLibrary);
    assert(library->fontLibrary);

    fixed_array_init(&library->spritesheets, spritesheetCapacity, sizeof(spritesheet), TAG_SYSTEM);
}

asset_handle asset_library_load_spritesheet(asset_library* library, const char* filepath, u32 spriteSize)
{
    assert(library && library->fontLibrary);

    // Loading:
    i32 width, height, channels;
	u8* data = stbi_load(filepath, &width, &height, &channels, SCA_AUTO_CHANNELS);
    assert(data);

    // Spritesheet:
    spritesheet s = { 0 }; 
    s.spriteSize = spriteSize;
    s.channels = channels;

    u64 spriteCount = (width * height ) / (spriteSize * spriteSize);

    // Empty characters:
    fixed_array_init(&s.sprites, spriteCount, sizeof(sprite), TAG_ASSETS);
  
    // Spliting:
    u32 spritesPerRow = width / spriteSize;
	u64 spriteByteSize = spriteSize * spriteSize * channels;
    for (u32 i = 0; i < spriteCount; ++i)
    {
        sprite character = { 0 };
        character.layer = i;
        fixed_array_init(&character.data, spriteByteSize, 1, TAG_ASSETS);

        u32 spriteX = i % spritesPerRow;
        u32 spriteY = i / spritesPerRow;

        for (u32 row = 0; row < spriteSize; ++row)
        {
            u64 srcByteOffset = (((spriteY * spriteSize + row) * width) + (spriteX * spriteSize)) * channels;
            fixed_array_push(&character.data, &data[srcByteOffset], spriteSize * channels);
        }

        fixed_array_push(&s.sprites, &character, 1);
    }

    fixed_array_push(&library->spritesheets, &s, 1);

	stbi_image_free(data);
    return fixed_array_size(&library->spritesheets) - 1;
}
asset_handle asset_library_load_sprites(asset_library* library, fixed_array* filepaths, u32 spriteSize, u32 channels)
{
    assert(library && library->fontLibrary);
    assert(filepaths);
    assert(spriteSize > 0);

    // Spritesheet:
    u32 sheetIndex = fixed_array_size(&library->spritesheets);
    fixed_array_push(&library->spritesheets, NULL, 1);

    spritesheet* s = fixed_array_get(&library->spritesheets, sheetIndex);
    s->spriteSize = spriteSize;
    s->channels = channels;

    // Loading:
    i32 size;
    bool firstImage = true;
    char* filepathData = (char*)filepaths->buffer;
    for (u32 i = 0; i < filepaths->current / sizeof(const char*); ++i)
    {
        char* currentFilepath = (char*)&filepathData[i];
        
        i32 width, height, channels;
	    u8* data = stbi_load(currentFilepath, &width, &height, &channels, 0);

        assert(width == height);

        if (firstImage)
        {
            size = width;   
            firstImage = false;
        }
        else if (size != width || size != height)
            log_critical_s("Images should have the same dimensions.", 40);


        sprite sprite = { 0 };
        sprite.layer = i;
        fixed_array_init(&sprite.data, width * height * channels, 1, TAG_ASSETS);
        fixed_array_push(&sprite.data, data, width * height * channels);
        
        fixed_array_push(&s->sprites, &sprite, 1);
    }

    return sheetIndex;
}

asset_handle asset_library_load_font(asset_library* library, const char* filepath, u32 height)
{
    assert(library && library->fontLibrary);
    
    u32 sheetIndex = fixed_array_size(&library->spritesheets);
    fixed_array_push(&library->spritesheets, NULL, 1);

    spritesheet* fontSpritesheet = fixed_array_get(&library->spritesheets, sheetIndex);
    fontSpritesheet->spriteSize = height;
    fontSpritesheet->channels = 1;

    FT_Face face;
    if (FT_New_Face(library->fontLibrary, filepath, 0, &face))
        return false;

    u64 fontCharacterAmount = (SCA_FONT_MAX_INDEX + 1);
    fixed_array_init(&fontSpritesheet->sprites, fontCharacterAmount, sizeof(sprite), TAG_ASSETS);
    for(u32 i = 0; i < fontCharacterAmount; ++i) 
    {
        sprite emptySprite = { 0 };
        fixed_array_push(&fontSpritesheet->sprites, &emptySprite, 1);
    }

    FT_Set_Pixel_Sizes(face, 0, height);

    u64 area = height * height;
    for(u16 i = 0; i < fontCharacterAmount; ++i)
    {
        // GLyph data:
        if (FT_Load_Char(face, i, FT_LOAD_DEFAULT))
            continue;

        if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_SDF))
            continue;

        FT_Bitmap* bitmap = &face->glyph->bitmap;
        u32 width  = bitmap->width;
        u32 rows   = bitmap->rows;
        u32 pitch  = abs(bitmap->pitch);

        if (width <= 0 || rows <= 0 || !bitmap->buffer)
        {
            sprite* character = (sprite*)fixed_array_get(&fontSpritesheet->sprites, i);
            character->layer = i;
            fixed_array_init(&character->data, area, 1, TAG_ASSETS);

            u8 zero = 0;
            for (u32 j = 0; j < area; ++j)
                fixed_array_push(&character->data, &zero, 1);

            continue;
        }

        sprite* character = (sprite*)fixed_array_get(&fontSpritesheet->sprites, i);
        character->layer = i;
        fixed_array_init(&character->data, area, 1, TAG_ASSETS);

        for (u32 y = 0; y < height; ++y)
        {
            u32 glyphY = (y * rows) / height;
            u8* sourceRow = (u8*)bitmap->buffer + (glyphY * pitch);

            for (u32 x = 0; x < height; ++x)
            {
                u32 glyphX = (x * width) / height;
                u8 texel = sourceRow[glyphX];
                
                fixed_array_push(&character->data, &texel, 1);
            }
        }
    }

    FT_Done_Face(face);
    return sheetIndex;
}

spritesheet* asset_library_get_spritesheet(asset_library* library, asset_handle handle)
{
    assert(library);
    return fixed_array_get(&library->spritesheets, handle);
}