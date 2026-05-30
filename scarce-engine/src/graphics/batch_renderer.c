#include "batch_renderer.h"
#include "buffer.h"
#include "cglm/vec4.h"
#include "defines.h"
#include "file_utils.h"
#include "memory/tag.h"
#include <assert.h>

#define GLEW_NO_GLU
#include <GL/glew.h>
#include <string.h>

#include "cglm/mat4.h"
#include "cglm/types.h"
#include "fixed_array.h"
#include "platform/platform.h"
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
    fixed_array_init(&renderer->cells, totalBytes, TAG_RENDERER);

    batch_renderer_cell emptyCell = { 0 };
    for (u32 i = 0; i < totalCells; ++i)
        fixed_array_push(&renderer->cells, &emptyCell, sizeof(batch_renderer_cell));

    if (renderer->charactersSSBO != buffer_invalid)
        rhi_destroy_buffer(renderer->rhi, renderer->charactersSSBO);

    renderer->charactersSSBO = rhi_create_buffer(renderer->rhi, totalBytes, BUFFER_COHERENT);
    zero_buffer(renderer);
}

void batch_renderer_init(batch_renderer* renderer, rhi rhi, window_handle window, shader_filepaths* shaders, i32 windowWidth, i32 windowHeight, u32 scale)
{
    renderer->charactersSSBO = buffer_invalid;
    renderer->cells.buffer = NULL;
    renderer->window = window;
    renderer->rhi = rhi;

    // Buffers:
    glm_mat4_identity(renderer->world.model);
    vec3 scaleVec = { scale, scale, 1.0f};
    glm_scale(renderer->world.model, scaleVec);
    rhi_update_buffer(renderer->rhi, renderer->worldUBO, &renderer->world, sizeof(batch_renderer_world), 0);

    batch_renderer_set_scale(renderer, scale);
    batch_renderer_on_resize(renderer, windowWidth, windowHeight);

    // Graphics:
    glCreateVertexArrays(1, &renderer->vao);

    shader_handle vertexShader, fragmentShader;
    {
        // Vertex shader:
        fixed_array contents = file_read_contents(shaders->vertexFilepath);
        vertexShader = rhi_create_shader(renderer->rhi, SHADER_VERTEX, contents.buffer);
        fixed_array_destroy(&contents);

        // Fragment shader:
        contents = file_read_contents(shaders->fragmentFilepath);
        fragmentShader = rhi_create_shader(renderer->rhi, SHADER_FRAGMENT, contents.buffer);
        fixed_array_destroy(&contents);
    }
        
    renderer->pipeline = rhi_create_pipeline(renderer->rhi, vertexShader, fragmentShader);
    renderer->worldUBO = rhi_create_buffer(renderer->rhi, sizeof(batch_renderer_world), BUFFER_DYNAMIC);
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

void batch_renderer_set_texture(batch_renderer* renderer, texture_handle texture)
{
    renderer->spritesheetTexture = texture;
}

void batch_renderer_set_world(batch_renderer* renderer, mat4 projection)
{
    glm_mat4_copy(projection, renderer->world.projection);
    rhi_update_buffer(renderer->rhi, renderer->worldUBO, &renderer->world, sizeof(batch_renderer_world), 0);
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
    rhi_update_buffer(renderer->rhi, renderer->worldUBO, &renderer->world, sizeof(batch_renderer_world), 0);
}

void batch_renderer_render(batch_renderer* renderer)
{
    rhi_update_buffer(renderer->rhi, renderer->charactersSSBO, renderer->cells.buffer, 
        renderer->gridWidth * renderer->gridHeight * sizeof(batch_renderer_cell), 
        0
    );

    rhi_begin_frame(renderer->rhi);
    
    rhi_bind_pipeline(renderer->rhi, renderer->pipeline);
    rhi_bind_ssbo(renderer->rhi, renderer->charactersSSBO, 0);
    rhi_bind_ubo(renderer->rhi, renderer->worldUBO, 1);
    rhi_bind_texture(renderer->rhi, renderer->spritesheetTexture, 2);

    u32 cellAmount = renderer->cells.current / sizeof(batch_renderer_cell);
    rhi_draw(renderer->rhi, cellAmount * 6);

    rhi_end_frame(renderer->rhi);
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

void batch_renderer_get_mouse_grid_position(window_handle windowHandle, batch_renderer* renderer, u32* x, u32* y)
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