#include "asset_upload.h"
#include "defines.h"
#include "texture.h"

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
		    information.internalFormat = SCA_TEXTURE_R8;
        if (spritesheet->channels == 2)
            information.internalFormat = SCA_TEXTURE_RG8;    
        if (spritesheet->channels == 3)
            information.internalFormat = SCA_TEXTURE_RGB8;    
        if (spritesheet->channels == 4)
            information.internalFormat = SCA_TEXTURE_RGBA8;
        
        information.type = SCA_2D_TEXTURE_ARRAY;

		information.generateMipmaps = false;

		information.wrapS = SCA_TEXTURE_REPEAT;
		information.wrapT = SCA_TEXTURE_REPEAT;
		information.minFilter = SCA_TEXTURE_NEAREST;
		information.magFilter = SCA_TEXTURE_NEAREST;
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

        texture_pixel_format format;
        if (spritesheet->channels == 1)
		    format = SCA_TEXTURE_RED;
        if (spritesheet->channels == 2)
            format = SCA_TEXTURE_RG;    
        if (spritesheet->channels == 3)
            format = SCA_TEXTURE_RGB;    
        if (spritesheet->channels == 4)
            format = SCA_TEXTURE_RGBA;    

        rhi_update_texture_array_layer(
            rhi, 
            spritesheetTexture, 
            &information, 
            sprite.layer, 
            offset, 
            format, 
            SCA_UNSIGNED_BYTE, 
            sprite.data.buffer
        );
	}

    return spritesheetTexture;
}