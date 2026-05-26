#include "asset_upload.h"

#define GLEW_NO_GLU
#include <GL/glew.h>

texture_handle upload_font_spritesheets(rhi rhi, spritesheet* spritesheet)
{
    u32 spriteAmount = spritesheet->sprites.current / sizeof(sprite);

    texture_information information = { 0 };
    {
        information.width = spritesheet->spriteSize * spriteAmount;
		information.height = spritesheet->spriteSize;
	    information.layerWidth = spritesheet->spriteSize;
	    information.layerHeight = spritesheet->spriteSize;
		information.depth = 1;
		information.mipLevels = 1;
		information.samples = 1;
		information.arrayLayers = spriteAmount;

        if (spritesheet->channels == 1)
		    information.format = GL_R8;
        if (spritesheet->channels == 2)
            information.format = GL_RG8;    
        if (spritesheet->channels == 3)
            information.format = GL_RGB8;    
        if (spritesheet->channels == 4)
            information.format = GL_RGBA8;    
        
        information.type = GL_TEXTURE_2D_ARRAY;

		information.generateMipmaps = false;

		information.wrapS = GL_REPEAT;
		information.wrapT = GL_REPEAT;
		information.minFilter = GL_NEAREST;
		information.magFilter = GL_NEAREST;
    }

    texture_handle spritesheetTexture = rhi_create_texture(rhi, &information); 

    sprite* characters = (sprite*)spritesheet->sprites.buffer;
    for(u32 i = 0; i < spriteAmount; ++i)
	{
        sprite sprite = characters[i];

		if(!sprite.data.buffer)
			continue;

        vec2 offset;
        {
            offset[0] = 0;
            offset[1] = 0;
        }

        u32 format;
        if (spritesheet->channels == 1)
		    format = GL_RED;
        if (spritesheet->channels == 2)
            format = GL_RG;    
        if (spritesheet->channels == 3)
            format = GL_RGB;    
        if (spritesheet->channels == 4)
            format = GL_RGBA;    

        rhi_update_texture_array_layer(
            rhi, 
            spritesheetTexture, 
            &information, 
            sprite.layer, 
            offset, 
            format, 
            GL_UNSIGNED_BYTE, 
            sprite.data.buffer
        );
	}

    return spritesheetTexture;
}