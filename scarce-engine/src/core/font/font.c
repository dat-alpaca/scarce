#include <stdlib.h>

#include "font.h"
#include "fixed_array.h"
#include "freetype/freetype.h"

bool font_loader_init(FT_Library* library)
{
    return !FT_Init_FreeType(library);
}

bool load_font(FT_Library* library, font* output, const char* filepath)
{
    output->maxHeight = 0;
    output->maxWidth = 0;

    if (FT_New_Face(*library, filepath, 0, &output->face))
		return false;

    FT_Set_Pixel_Sizes(output->face, 0, 64);
    fixed_array_init(&output->characters, ('~' - '!' + 1) * sizeof(font_character));

    for(unsigned char character = '!'; character <= '~'; ++character)
	{
		u32 index = character - '!';

        if (FT_Load_Char(output->face, character, FT_LOAD_DEFAULT))
			continue;

        if (FT_Render_Glyph(output->face->glyph, FT_RENDER_MODE_SDF))
            continue;

        FT_Bitmap* bitmap = &output->face->glyph->bitmap;
        u32 width  = bitmap->width;
        u32 rows   = bitmap->rows;
        u32 pitch = abs(bitmap->pitch);
        u64 totalBytes = (u64)width * rows;

        // Buffer copies
        font_character* characters = (font_character*)output->characters.buffer;
        fixed_array_init(&characters[index].data, totalBytes);
        output->characters.current += sizeof(font_character);

        if (totalBytes > 0 && characters[index].data.buffer != NULL)
        {   
            for (u32 i = 0; i < rows; ++i)
            {
                memcpy
                (
                    (unsigned char*)characters[index].data.buffer + (i * width),
                    bitmap->buffer + (i * pitch),
                    width
                );
            }
            characters[index].data.current = totalBytes;
        }

        characters[index].size[0]    = (float)bitmap->width;
        characters[index].size[1]    = (float)bitmap->rows;
        characters[index].bearing[0] = (float)output->face->glyph->bitmap_left;
        characters[index].bearing[1] = (float)output->face->glyph->bitmap_top;
        characters[index].advance    = output->face->glyph->advance.x >> 6;
        characters[index].layer      = index;
        characters[index].letter     = (char)character;

        if (bitmap->width > output->maxWidth)
            output->maxWidth = bitmap->width;

        if (bitmap->rows > output->maxHeight) 
            output->maxHeight = bitmap->rows;
	}

    return true;
}