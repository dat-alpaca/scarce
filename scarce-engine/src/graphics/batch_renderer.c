#include "batch_renderer.h"
#include "cglm/vec4.h"
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
#include "shader.h"

static void zero_buffer(batch_renderer* renderer)
{
    u32 totalCells = renderer->gridWidth * renderer->gridHeight;

    for (u32 i = 0; i < totalCells; ++i)
    {
        batch_renderer_cell* c = (batch_renderer_cell*)fixed_array_get(
            &renderer->cells, i, sizeof(batch_renderer_cell)
        );

        u32 x = i % renderer->gridWidth;
        u32 y = i / renderer->gridWidth;

        c->layer = ' ';
        c->position[0] = (float)x;
        c->position[1] = (float)y;
        c->color[3] = 1.0f;
        c->paintBackground = 0;
        c->backgroundColor[3] = 1.0f;
    }
}

static void reallocate_buffers(batch_renderer* renderer)
{
    if (renderer->cells.buffer != NULL)
        fixed_array_destroy(&renderer->cells);
    
    u32 totalCells = renderer->gridWidth * renderer->gridHeight;
    u32 totalBytes = totalCells * sizeof(batch_renderer_cell);
    fixed_array_init(&renderer->cells, totalBytes);

    batch_renderer_cell emptyCell = { 0 };
    for (u32 i = 0; i < totalCells; ++i)
        fixed_array_push(&renderer->cells, &emptyCell, sizeof(batch_renderer_cell));

    if (renderer->charactersSSBO != (gl_handle)-1)
        graphics_destroy_buffer(renderer->charactersSSBO);

    renderer->charactersSSBO = graphics_create_buffer(totalBytes, BUFFER_COHERENT);
    zero_buffer(renderer);
}

void batch_renderer_init(batch_renderer* renderer, window_handle* window, shader_filepaths* shaders, i32 windowWidth, i32 windowHeight, u32 scale)
{
    renderer->charactersSSBO = (gl_handle)-1;
    renderer->cells.buffer = NULL;
    renderer->window = window;

    // Buffers:
    glm_mat4_identity(renderer->world.model);
    vec3 scaleVec = { scale, scale, 1.0f};
    glm_scale(renderer->world.model, scaleVec);
    graphics_update_buffer(renderer->worldUBO, &renderer->world, sizeof(batch_renderer_world), 0);

    batch_renderer_set_scale(renderer, scale);
    batch_renderer_on_resize(renderer, windowWidth, windowHeight);

    // Graphics:
    glCreateVertexArrays(1, &renderer->vao);

    gl_handle vertexShader = read_shader(shaders->vertexFilepath, SHADER_VERTEX);
    gl_handle fragmentShader = read_shader(shaders->fragmentFilepath, SHADER_FRAGMENT);
    
    renderer->pipeline = graphics_create_pipeline(vertexShader, fragmentShader);
    renderer->worldUBO = graphics_create_buffer(sizeof(batch_renderer_world), BUFFER_DYNAMIC);
}

void batch_renderer_on_resize(batch_renderer* renderer, i32 windowWidth, i32 windowHeight)
{
    renderer->windowWidth = windowWidth;
    renderer->windowHeight = windowHeight;
    renderer->gridWidth = windowWidth / renderer->scale;
    renderer->gridHeight = windowHeight / renderer->scale;

    reallocate_buffers(renderer);

    mat4 project;
    glm_ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.f, 1.0f, project);
    batch_renderer_set_world(renderer, project);
}

void batch_renderer_set_texture(batch_renderer* renderer, gl_handle texture)
{
    renderer->spritesheetTexture = texture;
}

void batch_renderer_set_world(batch_renderer* renderer, mat4 projection)
{
    glm_mat4_copy(projection, renderer->world.projection);
    graphics_update_buffer(renderer->worldUBO, &renderer->world, sizeof(batch_renderer_world), 0);
}

void batch_renderer_set_scale(batch_renderer* renderer, u32 scale)
{
    renderer->scale = scale;
    renderer->gridWidth = renderer->windowWidth / scale;
    renderer->gridHeight = renderer->windowHeight / scale;

    reallocate_buffers(renderer);

    glm_mat4_identity(renderer->world.model);
    vec3 scaleVec = {scale, scale, 1.0f};
    glm_scale(renderer->world.model, scaleVec);
    graphics_update_buffer(renderer->worldUBO, &renderer->world, sizeof(batch_renderer_world), 0);
}

void batch_renderer_render(batch_renderer* renderer)
{
    graphics_update_buffer(renderer->charactersSSBO, renderer->cells.buffer, 
        renderer->gridWidth * renderer->gridHeight * sizeof(batch_renderer_cell), 
        0
    );

    graphics_begin_frame(renderer->vao);
    
    graphics_bind_pipeline(renderer->vao, renderer->pipeline);
    graphics_bind_ssbo(renderer->charactersSSBO, 0);
    graphics_bind_ubo(renderer->worldUBO, 1);
    graphics_bind_texture(renderer->spritesheetTexture, 2);

    u32 cellAmount = renderer->cells.current / sizeof(batch_renderer_cell);
    graphics_draw(cellAmount * 6);

    graphics_end_frame();
}

void batch_renderer_zero_buffer(batch_renderer* renderer)
{
    assert(renderer);
    zero_buffer(renderer);
}

batch_renderer_cell* batch_renderer_get_cell(batch_renderer* renderer, u32 x, u32 y)
{
    if (x >= renderer->gridWidth || y >= renderer->gridHeight) 
        return NULL;
    
    u32 index = y * renderer->gridWidth + x;
    return (batch_renderer_cell*)fixed_array_get
    (
        &renderer->cells, index, sizeof(batch_renderer_cell)
    );
}

void batch_renderer_set_cell(batch_renderer* renderer, batch_renderer_cell* cell, u32 x, u32 y)
{
    batch_renderer_cell* c = batch_renderer_get_cell(renderer, x, y);
    assert (c);

    glm_vec4_copy(cell->backgroundColor, c->backgroundColor);
    glm_vec4_copy(cell->color, c->color);
    c->layer = cell->layer;
}

void batch_renderer_get_mouse_grid_position(window_handle* windowHandle, batch_renderer* renderer, u32* x, u32* y)
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

    u32 gridX = (u32)(mouseX / renderer->scale);
    u32 gridY = (u32)(mouseY / renderer->scale);
    
    if (gridX >= renderer->gridWidth)
        gridX = renderer->gridWidth - 1;

    if (gridY >= renderer->gridHeight)
        gridY = renderer->gridHeight - 1;

    *x = gridX;
    *y = gridY;
}