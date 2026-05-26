#include "opengl_rhi.h"
#include "logging/logger.h"
#include "rhi/rhi.h"

#define GLEW_NO_GLU
#include <GL/glew.h>

rhi rhi_init()
{
    opengl_rhi* rhi = malloc(sizeof(opengl_rhi));
    return rhi;
}

void rhi_begin_frame(rhi rhi)
{
    opengl_rhi* gl = (opengl_rhi*)rhi;
    if (!gl->vao)
        glCreateVertexArrays(1, &gl->vao);

    glClearColor(35.f / 255.f, 39.f / 255.f, 38.f / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    opengl_rhi* oglrhi = (opengl_rhi*)rhi;
    glBindVertexArray(oglrhi->vao);
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
	
	return bufferID;
}
void rhi_update_buffer(rhi rhi, buffer_handle bufferHandle, void* buffer, u32 size, u32 offset)
{
    glNamedBufferSubData(bufferHandle, offset, size, buffer);
}
void rhi_bind_buffer(rhi rhi, buffer_handle bufferHandle, u32 binding, u32 stride)
{
    opengl_rhi* oglrhi = (opengl_rhi*)rhi;
    glVertexArrayVertexBuffer(oglrhi->vao, binding, bufferHandle, 0, stride);
}
void rhi_bind_element_buffer(rhi rhi, buffer_handle bufferHandle)
{
    opengl_rhi* oglrhi = (opengl_rhi*)rhi;
    glVertexArrayElementBuffer(oglrhi->vao, bufferHandle);
}
void rhi_destroy_buffer(rhi _, buffer_handle bufferHandle)
{
    glDeleteBuffers(1, &bufferHandle);
}

// Textures:
texture_handle rhi_create_texture(rhi _, texture_information* information)
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
void rhi_update_texture(rhi _, texture_handle texture, texture_information* information, void* data, u32 dataWidth, u32 dataHeight, u32 dataFormat, u32 dataFormatSize, u32 dataType)
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
void rhi_update_texture_array_layer(rhi _, texture_handle texture, texture_information* information, u32 layer, vec2 offset, u32 dataFormat, u32 dataType, void* data)
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
    return shaderID;
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
