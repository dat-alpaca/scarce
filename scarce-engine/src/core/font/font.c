#include <stdlib.h>

#include "font.h"
#include "defines.h"
#include "fixed_array.h"
#include "freetype/freetype.h"

bool font_loader_init(FT_Library* library)
{
    return !FT_Init_FreeType(library);
}

bool load_font(FT_Library* library, font* output, const char* filepath)
{
    assert(library && output && filepath);
    
    output->maxHeight = 0;
    output->maxWidth = 0;

    if (FT_New_Face(*library, filepath, 0, &output->face))
        return false;

    FT_Set_Pixel_Sizes(output->face, 0, 64);

    u32 charCount = (SCA_FONT_END_INDEX - SCA_FONT_START_INDEX + 1);
    fixed_array_init(&output->characters, charCount * sizeof(font_character));

    for(u32 i = 0; i < charCount; ++i) 
    {
        font_character emptyCharacter = { 0 };
        fixed_array_push(&output->characters, &emptyCharacter, sizeof(font_character));
    }

    for(unsigned char character = SCA_FONT_START_INDEX; character <= SCA_FONT_END_INDEX; ++character)
    {
        u32 index = character - SCA_FONT_START_INDEX;

        if (FT_Load_Char(output->face, character, FT_LOAD_DEFAULT))
            continue;

        if (FT_Render_Glyph(output->face->glyph, FT_RENDER_MODE_SDF))
            continue;

        FT_Bitmap* bitmap = &output->face->glyph->bitmap;
        u32 width  = bitmap->width;
        u32 rows   = bitmap->rows;
        u32 pitch  = abs(bitmap->pitch);
        u32 totalBytes = width * rows;

        font_character* destChar = (font_character*)fixed_array_get(&output->characters, index, sizeof(font_character));
        fixed_array_init(&destChar->data, totalBytes);

        if (totalBytes > 0 && destChar->data.buffer != NULL)
        {   
            for (u32 i = 0; i < rows; ++i)
            {
                void* sourceRow = bitmap->buffer + (i * pitch);
                fixed_array_push(&destChar->data, sourceRow, width);
            }
        }

        destChar->size[0]    = (float)width;
        destChar->size[1]    = (float)rows;
        destChar->bearing[0] = (float)output->face->glyph->bitmap_left;
        destChar->bearing[1] = (float)output->face->glyph->bitmap_top;
        destChar->advance    = output->face->glyph->advance.x >> 6;
        destChar->layer      = index;
        destChar->letter     = (char)character;

        if (width > output->maxWidth)  
            output->maxWidth = width;
        
        if (rows > output->maxHeight) 
            output->maxHeight = rows;
    }

    return true;
}