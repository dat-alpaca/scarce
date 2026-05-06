#include "text_renderer.h"
#include "defines.h"
#include <assert.h>

#define GLEW_NO_GLU
#include <GL/glew.h>
#include <string.h>

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

    platform_read_file(file, buffer.buffer, fileSize);
    buffer.current = fileSize;

    char null = '\0';
    fixed_array_push(&buffer, &null, 1);

    gl_handle shader = graphics_create_shader(type, buffer.buffer);
    platform_close_file(file);
    fixed_array_destroy(&buffer);

    return shader;
}

static void zero_buffer(text_renderer* renderer)
{
    u32 totalCells = renderer->gridWidth * renderer->gridHeight;

    for (u32 i = 0; i < totalCells; ++i)
    {
        text_renderer_character* c = (text_renderer_character*)fixed_array_get(
            &renderer->characters, i, sizeof(text_renderer_character)
        );

        u32 x = i % renderer->gridWidth;
        u32 y = i / renderer->gridWidth;

        c->position[0] = (float)x;
        c->position[1] = (float)y;
        c->uvSize[0] = 0;
        c->uvSize[1] = 0;
        c->color[3] = 1.0f;
        c->paintBackground = 0;
        c->backgroundColor[3] = 1.0f;
    }
}

static void reallocate_buffers(text_renderer* renderer)
{
    if (renderer->characters.buffer != NULL)
        fixed_array_destroy(&renderer->characters);
    
    u32 totalCells = renderer->gridWidth * renderer->gridHeight;
    u32 totalBytes = totalCells * sizeof(text_renderer_character);
    fixed_array_init(&renderer->characters, totalBytes);

    text_renderer_character emptyCell = { 0 };
    for (u32 i = 0; i < totalCells; ++i)
        fixed_array_push(&renderer->characters, &emptyCell, sizeof(text_renderer_character));

    if (renderer->charactersSSBO != (gl_handle)-1)
        graphics_destroy_buffer(renderer->charactersSSBO);

    renderer->charactersSSBO = graphics_create_buffer(totalBytes, BUFFER_COHERENT);
    zero_buffer(renderer);
}

void text_renderer_init(text_renderer* renderer, font* font, shader_filepaths* shaders, i32 windowWidth, i32 windowHeight, u32 characterSize)
{
    renderer->loadedFont = font;
    renderer->charactersSSBO = (gl_handle)-1;
    renderer->characters.buffer = NULL;

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

    text_renderer_on_resize(renderer, windowWidth, windowHeight);

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

    reallocate_buffers(renderer);

    mat4 project;
    glm_ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.f, 1.0f, project);
    text_renderer_set_world(renderer, project);
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
    graphics_bind_ubo(renderer->worldUBO, 1);
    graphics_bind_texture(renderer->fontTexture, 2);

    graphics_draw(renderer->characters.current / sizeof(text_renderer_character) * 6);

    graphics_end_frame();
}

void text_renderer_zero_buffer(text_renderer* renderer)
{
    assert(renderer);
    zero_buffer(renderer);
}

text_renderer_character* text_renderer_get_character(text_renderer* renderer, u32 x, u32 y)
{
    if (x >= renderer->gridWidth || y >= renderer->gridHeight) 
        return NULL;
    
    u32 index = y * renderer->gridWidth + x;
    return (text_renderer_character*)fixed_array_get
    (
        &renderer->characters, index, sizeof(text_renderer_character)
    );
}

void text_renderer_set_character_letter(text_renderer* renderer, u32 x, u32 y, char letter)
{
    text_renderer_character* c = text_renderer_get_character(renderer, x, y);
    assert (c && letter >= SCA_FONT_START_INDEX && letter < SCA_FONT_END_INDEX);
    
    font_character* fontChars = (font_character*)renderer->loadedFont->characters.buffer;
    font_character* fc = &fontChars[letter - SCA_FONT_START_INDEX];

    c->uvSize[0] = fc->size[0] / renderer->loadedFont->maxWidth;
    c->uvSize[1] = fc->size[1] / renderer->loadedFont->maxHeight;
    c->layer = fc->layer;
}

void text_renderer_set_character_color(text_renderer* renderer, u32 x, u32 y, float r, float g, float b)
{
    text_renderer_character* c = text_renderer_get_character(renderer, x, y);
    assert(c);

    c->color[0] = r;
    c->color[1] = g;
    c->color[2] = b;
    c->color[3] = 1.0f;
}

void text_renderer_set_character_background_color(text_renderer* renderer, u32 x, u32 y, float r, float g, float b, bool paint)
{
    text_renderer_character* c = text_renderer_get_character(renderer, x, y);
    assert(c);

    c->backgroundColor[0] = r;
    c->backgroundColor[1] = g;
    c->backgroundColor[2] = b;
    c->backgroundColor[3] = 1.0f;
    c->paintBackground = paint;
}

u32 text_renderer_width(text_renderer* renderer)
{
    return renderer->gridWidth;
}
u32 text_renderer_height(text_renderer* renderer)
{
    return renderer->gridHeight;    
}
u32 text_renderer_window_width(text_renderer* renderer)
{
    return renderer->windowWidth;    
}
u32 text_renderer_window_height(text_renderer* renderer)
{
    return renderer->windowHeight;    
}
u32 text_renderer_character_size(text_renderer* renderer)
{
    return renderer->characterSize;
}

void text_renderer_get_mouse_grid_position(window_handle* windowHandle, text_renderer* renderer, u32* x, u32* y)
{
    double mouseX, mouseY;
    window_get_mouse_position(windowHandle, &mouseX, &mouseY);

    if (mouseX < 0)
        mouseX = 0;
    
    if (mouseY < 0)
        mouseY = 0;

    if (mouseX >= renderer->windowWidth)
        mouseX = renderer->windowWidth - 1;

    if (mouseY >= renderer->windowHeight)
        mouseY = renderer->windowHeight - 1;

    u32 gridX = (u32)(mouseX / renderer->characterSize);
    u32 gridY = (u32)(mouseY / renderer->characterSize);
    
    if (gridX >= renderer->gridWidth)
        gridX = renderer->gridWidth - 1;

    if (gridY >= renderer->gridHeight)
        gridY = renderer->gridHeight - 1;

    *x = gridX;
    *y = gridY;
}