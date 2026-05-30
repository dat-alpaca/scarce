#include "opengl_rhi.h"
#include "defines.h"
#include "logging/logger.h"
#include "memory/memory_system.h"
#include "memory/tag.h"
#include "platform/platform.h"
#include "rhi/rhi.h"
#include "shader.h"
#include "texture.h"

#define GLEW_NO_GLU
#include <GL/glew.h>

rhi rhi_init()
{
	opengl_rhi* gl = (opengl_rhi*)sca_allocate(TAG_SYSTEM, NULL, sizeof(opengl_rhi), 1);
	gl->vao = gl_invalid;

    return gl;
}

void rhi_begin_frame(rhi rhi)
{
    opengl_rhi* gl = (opengl_rhi*)rhi;
	if (gl->vao == gl_invalid)
		glCreateVertexArrays(1, &gl->vao);

    glClearColor(35.f / 255.f, 39.f / 255.f, 38.f / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(gl->vao);
}
void rhi_end_frame(rhi rhi)
{
    /* Blank */
}
void rhi_set_viewport(rhi _, i32 x, i32 y, i32 width, i32 height)
{
    glViewport(x, y, width, height);
}

// Buffers:
buffer_handle rhi_create_buffer(rhi rhi, u32 size, buffer_usage usage)
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
	
	return (buffer_handle)bufferID;
}
void rhi_update_buffer(rhi rhi, buffer_handle bufferHandle, void* buffer, u32 size, u32 offset)
{
    glNamedBufferSubData(bufferHandle, offset, size, buffer);
}
void rhi_bind_buffer(rhi rhi, buffer_handle bufferHandle, u32 binding, u32 stride)
{
    opengl_rhi* gl = (opengl_rhi*)rhi;
    glVertexArrayVertexBuffer(gl->vao, binding, bufferHandle, 0, stride);
}
void rhi_bind_element_buffer(rhi rhi, buffer_handle bufferHandle)
{
    opengl_rhi* gl = (opengl_rhi*)rhi;
    glVertexArrayElementBuffer(gl->vao, bufferHandle);
}
void rhi_destroy_buffer(rhi _, buffer_handle bufferHandle)
{
    glDeleteBuffers(1, &bufferHandle);
}

// Textures:
static gl_handle get_wrap_mode(texture_wrap wrap)
{
	switch (wrap)
	{
		case SCA_TEXTURE_REPEAT: return GL_REPEAT;
		case SCA_TEXTURE_CLAMP_EDGE: return GL_CLAMP_TO_EDGE;
		case SCA_TEXTURE_MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
	}
	return invalid_handle;
}

static gl_handle get_filter(texture_filter filter)
{
	switch (filter)
	{
		case SCA_TEXTURE_NEAREST: return GL_NEAREST;
		case SCA_TEXTURE_LINEAR: return GL_LINEAR;
		case SCA_TEXTURE_NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
		case SCA_TEXTURE_LINEAR_MIPMAP_NEAREST: return GL_LINEAR_MIPMAP_NEAREST;
		case SCA_TEXTURE_NEAREST_MIPMAP_LINEAR: return GL_NEAREST_MIPMAP_LINEAR;
		case SCA_TEXTURE_LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
	}
	return invalid_handle;
}

texture_handle rhi_create_texture(rhi _, texture_information* information)
{
    gl_handle textureID;

	u32 textureType;
	switch (information->type)
	{
		case SCA_2D_TEXTURE: textureType = GL_TEXTURE_2D;
		case SCA_2D_TEXTURE_ARRAY: textureType = GL_TEXTURE_2D_ARRAY;
	}

	glCreateTextures(textureType, 1, &textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

	{
		bool generateMipmaps = information->generateMipmaps;

		glTextureParameteri(textureID, GL_TEXTURE_MAX_LEVEL, information->arrayLayers);
		glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, get_wrap_mode(information->wrapS));
		glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, get_wrap_mode(information->wrapT));

		glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, get_filter(information->minFilter));
		glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, get_filter(information->magFilter));

		if (generateMipmaps)
			glGenerateTextureMipmap(textureID);
	}

	u32 internalFormat;
	switch (information->internalFormat)
	{
		case SCA_TEXTURE_R8: internalFormat = GL_R8; break;
    	case SCA_TEXTURE_RG8: internalFormat = GL_RG8; break;
    	case SCA_TEXTURE_RGB8: internalFormat = GL_RGB8; break;
    	case SCA_TEXTURE_RGBA8: internalFormat = GL_RGBA8; break;	
	}

	switch (information->type)
	{
		case SCA_2D_TEXTURE:
			glTextureStorage2D(textureID, information->mipLevels, internalFormat, information->width, information->height);
			break;

		case SCA_2D_TEXTURE_ARRAY:
			glTextureStorage3D(textureID, information->mipLevels, internalFormat, information->layerWidth, information->layerHeight, information->arrayLayers);
			break;
	}

	return (texture_handle)textureID;
}
void rhi_update_texture(rhi _, texture_handle texture, texture_information* information, void* data, u32 dataWidth, u32 dataHeight, texture_pixel_format dataFormat, u32 dataFormatSize, type dataType)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	gl_handle pixelFormat;
	switch(dataFormat)
	{
		case SCA_TEXTURE_RED: pixelFormat = GL_RED; break;
		case SCA_TEXTURE_RG: pixelFormat = GL_RG; break;
		case SCA_TEXTURE_RGB: pixelFormat = GL_RGB; break;
		case SCA_TEXTURE_RGBA: pixelFormat = GL_RGBA;  break;
	}

	gl_handle type;
	switch (dataType)
	{
		case SCA_UNSIGNED_BYTE: type = GL_UNSIGNED_BYTE; break;
	}

	switch (information->type)
    {
    	case SCA_2D_TEXTURE:
        {
			glTextureSubImage2D(
				texture, 
                0, 0, 0, 
                dataWidth, dataHeight, pixelFormat, type, data
			);

        } break;
		
		case SCA_2D_TEXTURE_ARRAY:
		{
			glPixelStorei(GL_UNPACK_ROW_LENGTH, dataWidth);
			for(u32 i = 0; i < information->arrayLayers; ++i)
			{
				glTextureSubImage3D(
					texture,
					0,
					0, 0, i,
					information->layerWidth, information->layerHeight, 1,
					pixelFormat,
					type,
					data + i * (information->layerWidth * dataFormatSize)
				);
			}
		} break;
    }

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}
void rhi_update_texture_array_layer(rhi _, texture_handle texture, texture_information* information, u32 layer, vec2 offset, texture_pixel_format dataFormat, type dataType, void* data)
{
	gl_handle pixelFormat;
	switch(dataFormat)
	{
		case SCA_TEXTURE_RED: pixelFormat = GL_RED; break;
		case SCA_TEXTURE_RG: pixelFormat = GL_RG; break;
		case SCA_TEXTURE_RGB: pixelFormat = GL_RGB; break;
		case SCA_TEXTURE_RGBA: pixelFormat = GL_RGBA;  break;
	}

	gl_handle type;
	switch (dataType)
	{
		case SCA_UNSIGNED_BYTE: type = GL_UNSIGNED_BYTE; break;
	}

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTextureSubImage3D(
		texture,
		0,
		offset[0], offset[1], layer,
		information->layerWidth, information->layerHeight, 1,
		pixelFormat,
		type,
		data
	);

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}
void rhi_destroy_texture(rhi _, texture_handle texture)
{
    glDeleteTextures(1, &texture);
}

// Pipeline:
static inline void check_compile_error(gl_handle handle)
{
	int success;
	char infoLog[256];

	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(handle, 256, NULL, infoLog);
        log_error_s(infoLog, 256);
    }
}

static inline void check_program_link_errors(u32 program)
{
    int success;
	char infoLog[256];

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 256, NULL, infoLog);
        log_error_s(infoLog, 256);
    }
}

shader_handle rhi_create_shader(rhi rhi, shader_type type, char* shaderContents)
{
	u32 shaderType = 0;
    if (type == SHADER_VERTEX)
        shaderType = GL_VERTEX_SHADER;
    else if (type == SHADER_FRAGMENT)
        shaderType = GL_FRAGMENT_SHADER;

    gl_handle shaderID = glCreateShader(shaderType);
    
    const char* source = shaderContents;
    glShaderSource(shaderID, 1, &source, NULL);
    glCompileShader(shaderID);
    
    check_compile_error(shaderID);
    return (shader_handle)shaderID;
}
pipeline rhi_create_pipeline(rhi rhi, shader_handle vertexShader, shader_handle fragmentShader)
{
	pipeline pipeline = { 0 };
    pipeline.handle = glCreateProgram();
	
    glAttachShader(pipeline.handle, vertexShader);
    glAttachShader(pipeline.handle, fragmentShader);
    glLinkProgram(pipeline.handle);
    check_program_link_errors(pipeline.handle);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return pipeline;
}
void rhi_bind_pipeline(rhi rhi, pipeline pipeline)
{
    opengl_rhi* gl = (opengl_rhi*)rhi;
	gl_handle vao = gl->vao;

	glUseProgram(pipeline.handle);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    input_stage* stage = &pipeline.inputStage;
    for(u32 i = 0; i < stage->bindingSize; ++i)
    {
        binding binding = stage->bindings[i];

        for(u32 j = 0; j < stage->attributeSize; ++j)
        {
            attribute attribute = stage->attributes[j];

            if (attribute.binding != binding.binding)
				continue;

            glEnableVertexArrayAttrib(vao, attribute.location);

			glVertexArrayAttribFormat(
				vao, attribute.location, attribute.formatSize,
				attribute.format, false, attribute.offset
			);

			glVertexArrayAttribBinding(vao, attribute.location, binding.binding);
        }

        glVertexArrayBindingDivisor(vao, binding.binding, 0);
    }
}

void rhi_bind_ssbo(rhi rhi, buffer_handle buffer, u32 binding)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer);
}
void rhi_bind_ubo(rhi rhi, buffer_handle buffer, u32 binding)
{
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, buffer);
}
void rhi_bind_texture(rhi rhi, texture_handle texture, u32 binding)
{
	glBindTextureUnit(binding, texture);
}

void rhi_draw(rhi rhi, u32 vertexCount)
{
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}
