#include "pipeline.h"
#include "graphics/graphics.h"

#define GLEW_NO_GLU
#include <GL/glew.h>
#include <stdio.h>

static void check_compile_error(gl_handle handle)
{
	int success;
	char infoLog[256];

	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(handle, 256, NULL, infoLog);
        fprintf(stderr, "%s\n", infoLog);
    }
}

static void check_program_link_errors(u32 program)
{
    int success;
	char infoLog[256];

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 256, NULL, infoLog);
        fprintf(stderr, "%s\n", infoLog);
    }
}

gl_handle graphics_create_shader(shader_type type, char* shaderContents)
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

pipeline graphics_create_pipeline(gl_handle vertexShader, gl_handle fragmentShader)
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

void graphics_bind_pipeline(gl_handle vao, pipeline pipeline)
{
    glUseProgram(pipeline.handle);

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

void graphics_bind_ssbo(gl_handle buffer, u32 binding)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer);
}
void graphics_bind_ubo(gl_handle buffer, u32 binding)
{
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, buffer);
}
void graphics_bind_texture(gl_handle texture, u32 binding)
{
    glBindTextureUnit(binding, texture);
}