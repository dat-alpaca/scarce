#include "graphics.h"

#define GLEW_NO_GLU
#include <GL/glew.h>

void graphics_begin_frame(gl_handle vao)
{
    glClearColor(35.f / 255.f, 39.f / 255.f, 38.f / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vao);
}
void graphics_end_frame()
{
	/* Blank */
}

// Buffers:
gl_handle graphics_create_buffer(u32 size, buffer_usage usage)
{
    u32 flags = GL_DYNAMIC_STORAGE_BIT;

	if (usage == BUFFER_READ_ONLY)
		flags |= GL_MAP_READ_BIT;

	if (usage == BUFFER_WRITE_ONLY)
		flags |= GL_MAP_WRITE_BIT;

	if (usage == BUFFER_PERSISTENT)
	{
		flags |= GL_MAP_PERSISTENT_BIT;
	    flags |= GL_MAP_READ_BIT;
		flags |= GL_MAP_WRITE_BIT;
		flags |= GL_DYNAMIC_STORAGE_BIT;
	}
    
	if (usage == BUFFER_COHERENT)
	{
		flags |= GL_MAP_COHERENT_BIT;
		flags |= GL_MAP_PERSISTENT_BIT;
		flags |= GL_MAP_READ_BIT;
		flags |= GL_MAP_WRITE_BIT;
		flags |= GL_DYNAMIC_STORAGE_BIT;
	}

    gl_handle bufferID;
	glCreateBuffers(1, &bufferID);
	glNamedBufferStorage(bufferID, size, NULL, flags);
	
	return bufferID;
}

void graphics_update_buffer(gl_handle bufferHandle, void* buffer, u32 size, u32 offset)
{
    glNamedBufferSubData(bufferHandle, offset, size, buffer);
}

void graphics_bind_buffer(gl_handle vao, gl_handle bufferHandle, u32 binding, u32 stride)
{
    glVertexArrayVertexBuffer(vao, binding, bufferHandle, 0, stride);
}

void graphics_bind_element_buffer(gl_handle vao, gl_handle bufferHandle)
{
    glVertexArrayElementBuffer(vao, bufferHandle);
}

void graphics_destroy_buffer(gl_handle bufferHandle)
{
    glDeleteBuffers(1, &bufferHandle);
}

gl_handle graphics_create_texture(texture_information* information)
{
	gl_handle textureID;
	glCreateTextures(information->type, 1, &textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

	{
		bool generateMipmaps = information->generateMipmaps;

		glTextureParameteri(textureID, GL_TEXTURE_MAX_LEVEL, information->arrayLayers);
		glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, information->wrapS);
		glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, information->wrapT);

		glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, information->minFilter);
		glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, information->magFilter);

		if (generateMipmaps)
			glGenerateTextureMipmap(textureID);
	}

	switch (information->type)
	{
		case GL_TEXTURE_2D:
			glTextureStorage2D(textureID, information->mipLevels, information->format, information->width, information->height);
			break;

		case GL_TEXTURE_2D_ARRAY:
			glTextureStorage3D(textureID, information->mipLevels, information->format, information->layerWidth, information->layerHeight, information->arrayLayers);
			break;
	}

	return textureID;
}

void graphics_update_texture(gl_handle texture, texture_information* information, void* data, u32 dataWidth, u32 dataHeight, u32 dataFormat, u32 dataFormatSize, u32 dataType)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    switch (information->type)
    {
    	case GL_TEXTURE_2D:
        {
			glTextureSubImage2D(
				texture, 
                0, 0, 0, 
                dataWidth, dataHeight, dataFormat, dataType, data
			);

        } break;
		
		case GL_TEXTURE_2D_ARRAY:
		{
			glPixelStorei(GL_UNPACK_ROW_LENGTH, dataWidth);
			for(u32 i = 0; i < information->arrayLayers; ++i)
			{
				glTextureSubImage3D(
					texture,
					0,
					0, 0, i,
					information->layerWidth, information->layerHeight, 1,
					dataFormat,
					dataType,
					data + i * (information->layerWidth * dataFormatSize)
				);
			}
		} break;
    }

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void graphics_update_texture_array_layer(gl_handle texture, texture_information* information, u32 layer, vec2 offset, u32 dataFormat, u32 dataType, void* data)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTextureSubImage3D(
		texture,
		0,
		offset[0], offset[1], layer,
		information->layerWidth, information->layerHeight, 1,
		dataFormat,
		dataType,
		data
	);

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void graphics_destroy_texture(gl_handle texture)
{
	glDeleteTextures(1, &texture);
}

void graphics_draw(u32 vertexCount)
{
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}