#include "text_renderer.h"

#define GLEW_NO_GLU
#include <GL/glew.h>
#include <string.h>
#include <stdio.h>

#include "cglm/mat4.h"
#include "cglm/types.h"
#include "fixed_array.h"
#include "graphics/graphics.h"
#include "platform/platform.h"
#include "pipeline.h"

static gl_handle read_shader(const char* filepath, shader_type type)
{
    file_descriptor file = platform_open_file(filepath, SCA_FILE_READ | SCA_FILE_CREATE);
    if (file == invalid_file_descriptor)
        return -1;

    u32 fileSize = platform_file_size(file);
    if (fileSize <= 0)
        return -1;
    
    fixed_array buffer = { 0 };
    fixed_array_init(&buffer, fileSize + 1);

    char* bufferData = (char*)buffer.buffer;
    platform_read_file(file, buffer.buffer, fileSize);
    bufferData[fileSize] = '\0';

    gl_handle shader = graphics_create_shader(type, buffer.buffer);
    platform_close_file(file);
    fixed_array_destroy(&buffer);

    return shader;
}

static void zero_buffer(text_renderer* renderer)
{
    text_renderer_character* characters = (text_renderer_character*)renderer->characters.buffer;
    
    u32 gridWidth = renderer->gridWidth;
    u32 gridHeight = renderer->gridHeight;
    for (u32 x = 0; x < gridWidth; ++x)
    {
        for (u32 y = 0; y < gridHeight; ++y)
        {
            characters[y * gridWidth + x].position[0] = x;
            characters[y * gridWidth + x].position[1] = y;
            characters[y * gridWidth + x].uvSize[0] = 0;
            characters[y * gridWidth + x].uvSize[1] = 0;
            characters[y * gridWidth + x].color[0] = 0.0f;
            characters[y * gridWidth + x].color[1] = 0.0f;
            characters[y * gridWidth + x].color[2] = 0.0f;
            characters[y * gridWidth + x].color[3] = 1.0f;
            characters[y * gridWidth + x].backgroundColor[0] = 0.0f;
            characters[y * gridWidth + x].backgroundColor[1] = 0.0f;
            characters[y * gridWidth + x].backgroundColor[2] = 0.0f;
            characters[y * gridWidth + x].backgroundColor[3] = 1.0f;
            characters[y * gridWidth + x].layer = 0;
            characters[y * gridWidth + x].paintBackground = 0;
        }
    }
}

static void reallocate_buffers(text_renderer* renderer)
{
    bool wasValidRenderer = false;
    fixed_array bufferCopy = { 0 };

    if (renderer->characters.buffer)
    {
        fixed_array_init(&bufferCopy, renderer->characters.capacity);
        memcpy(bufferCopy.buffer, renderer->characters.buffer, renderer->characters.capacity);
        bufferCopy.capacity = renderer->characters.capacity;

        fixed_array_destroy(&renderer->characters);
        wasValidRenderer = true;
    }

    fixed_array_init(&renderer->characters, renderer->gridWidth * renderer->gridHeight * sizeof(text_renderer_character));
    
    if (!wasValidRenderer)
    {
        zero_buffer(renderer);
        renderer->characters.current = renderer->gridWidth * renderer->gridHeight * sizeof(text_renderer_character);
    }

    if (bufferCopy.capacity)
    {
        if (renderer->characters.capacity <= bufferCopy.capacity)
            memcpy(renderer->characters.buffer, bufferCopy.buffer, renderer->characters.capacity);
        else
            memcpy(renderer->characters.buffer, bufferCopy.buffer, bufferCopy.capacity);

        renderer->characters.current = renderer->characters.capacity;

        fixed_array_destroy(&bufferCopy);
    }

    if (renderer->charactersSSBO != (gl_handle)-1)
        graphics_destroy_buffer(renderer->charactersSSBO);

    renderer->charactersSSBO = graphics_create_buffer(renderer->gridWidth * renderer->gridHeight * sizeof(text_renderer_character), BUFFER_COHERENT);
}

void text_renderer_init(text_renderer* renderer, font* font, shader_filepaths* shaders, i32 windowWidth, i32 windowHeight, u32 characterSize)
{
    renderer->characters.buffer = NULL;
    renderer->loadedFont = font;
    renderer->charactersSSBO = -1;

    // Buffers:
    glm_mat4_identity(renderer->world.model);
    vec3 scaleVec = {characterSize, characterSize, 1.0f};
    glm_scale(renderer->world.model, scaleVec);
    graphics_update_buffer(renderer->worldUBO, &renderer->world, sizeof(text_renderer_world), 0);

    renderer->characterSize = characterSize;
    renderer->windowWidth = windowWidth;
    renderer->windowHeight = windowHeight;
    renderer->gridWidth = windowWidth / renderer->characterSize;
    renderer->gridHeight = windowHeight / renderer->characterSize;

    reallocate_buffers(renderer);

    // Graphics:
    glCreateVertexArrays(1, &renderer->vao);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    gl_handle vertexShader = read_shader(shaders->vertexFilepath, SHADER_VERTEX);
    gl_handle fragmentShader = read_shader(shaders->fragmentFilepath, SHADER_FRAGMENT);
    
    renderer->pipeline = graphics_create_pipeline(vertexShader, fragmentShader);
    renderer->worldUBO = graphics_create_buffer(sizeof(text_renderer_world), BUFFER_DYNAMIC);
}

void text_renderer_on_resize(text_renderer* renderer, i32 windowWidth, i32 windowHeight)
{
    renderer->windowWidth = windowWidth;
    renderer->windowHeight = windowHeight;
    renderer->gridWidth = windowWidth / renderer->characterSize;
    renderer->gridHeight = windowHeight / renderer->characterSize;
}

void text_renderer_set_texture(text_renderer* renderer, gl_handle texture)
{
    renderer->fontTexture = texture;
}

void text_renderer_set_world(text_renderer* renderer, mat4 projection)
{
    glm_mat4_copy(projection, renderer->world.projection);
    graphics_update_buffer(renderer->worldUBO, &renderer->world, sizeof(text_renderer_world), 0);
}

void text_renderer_set_character_size(text_renderer* renderer, u32 characterSize)
{
    renderer->characterSize = characterSize;
    renderer->gridWidth = renderer->windowWidth / renderer->characterSize;
    renderer->gridHeight = renderer->windowHeight / renderer->characterSize;

    reallocate_buffers(renderer);

    glm_mat4_identity(renderer->world.model);
    vec3 scaleVec = {characterSize, characterSize, 1.0f};
    glm_scale(renderer->world.model, scaleVec);
    graphics_update_buffer(renderer->worldUBO, &renderer->world, sizeof(text_renderer_world), 0);
}

void text_renderer_render(text_renderer* renderer)
{
    graphics_update_buffer(renderer->charactersSSBO, renderer->characters.buffer, 
        renderer->gridWidth * renderer->gridHeight * sizeof(text_renderer_character), 
        0
    );

    graphics_begin_frame(renderer->vao);
    
    graphics_bind_pipeline(renderer->vao, renderer->pipeline);
    graphics_bind_ssbo(renderer->charactersSSBO, 0);
    graphics_bind_texture(renderer->fontTexture, 2);
    graphics_bind_ubo(renderer->worldUBO, 1);

    graphics_draw(renderer->characters.current / sizeof(text_renderer_character) * 6);

    graphics_end_frame();
}

void text_renderer_set_character_letter(text_renderer* renderer, u32 x, u32 y, char letter)
{
    text_renderer_character* characters = (text_renderer_character*)renderer->characters.buffer;
    
    font_character* fontCharacterBuffer = (font_character*)renderer->loadedFont->characters.buffer;
    font_character character = fontCharacterBuffer[letter - '!'];
    
    characters[y * renderer->gridWidth + x].uvSize[0] = character.size[0] / renderer->loadedFont->maxWidth;
    characters[y * renderer->gridWidth + x].uvSize[1] = character.size[1] / renderer->loadedFont->maxHeight;
    characters[y * renderer->gridWidth + x].layer = character.layer;
}

void text_renderer_set_character_color(text_renderer* renderer, u32 x, u32 y, float r, float g, float b)
{
    text_renderer_character* characters = (text_renderer_character*)renderer->characters.buffer;
    characters[y * renderer->gridWidth + x].color[0] = r;
    characters[y * renderer->gridWidth + x].color[1] = g;
    characters[y * renderer->gridWidth + x].color[2] = b;
    characters[y * renderer->gridWidth + x].color[3] = 1.0f;
}

void text_renderer_set_character_background_color(text_renderer* renderer, u32 x, u32 y, float r, float g, float b, bool paint)
{
    text_renderer_character* characters = (text_renderer_character*)renderer->characters.buffer;
    characters[y * renderer->gridWidth + x].backgroundColor[0] = r;
    characters[y * renderer->gridWidth + x].backgroundColor[1] = g;
    characters[y * renderer->gridWidth + x].backgroundColor[2] = b;
    characters[y * renderer->gridWidth + x].backgroundColor[3] = 1.0f;
    characters[y * renderer->gridWidth + x].paintBackground = paint;
}

u32 text_renderer_width(text_renderer* renderer)
{
    return renderer->gridWidth;
}
u32 text_renderer_height(text_renderer* renderer)
{
    return renderer->gridHeight;    
}