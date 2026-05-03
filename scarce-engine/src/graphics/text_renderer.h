#pragma once
#include <cglm/cglm.h>
#include "core/font/font.h"
#include "graphics/graphics.h"
#include "core/fixed_array.h"
#include "graphics/pipeline.h"
#include "platform/platform.h"

typedef struct
{
    vec2 position;
    vec2 uvSize;
    u32 layer;
    u32 paintBackground;
    vec4 color;
    vec4 backgroundColor;
} text_renderer_character;

typedef struct
{
    mat4 model;
    mat4 projection;
} text_renderer_world;

typedef struct
{
    font* loadedFont;
    fixed_array characters;
    
    text_renderer_world world;
    pipeline pipeline;
    
    gl_handle vao;
    gl_handle VBO;
    gl_handle worldUBO;
    gl_handle charactersSSBO;
    
    gl_handle fontTexture;
    
    u32 windowWidth;
    u32 windowHeight;
    u32 gridWidth;
    u32 gridHeight;
    u32 characterSize;
} text_renderer;

typedef struct
{
    const char* vertexFilepath;
    const char* fragmentFilepath;
} shader_filepaths;

void text_renderer_init(text_renderer* renderer, font* font, shader_filepaths* shaders, i32 windowWidth, i32 windowHeight, u32 characterSize);
void text_renderer_on_resize(text_renderer* renderer, i32 windowWidth, i32 windowHeight);
void text_renderer_set_texture(text_renderer* renderer, gl_handle texture);
void text_renderer_set_world(text_renderer* renderer, mat4 projection);
void text_renderer_set_character_size(text_renderer* renderer, u32 characterSize);

void text_renderer_render(text_renderer* renderer);

void text_renderer_set_character_letter(text_renderer* renderer, u32 x, u32 y, char letter);
void text_renderer_set_character_color(text_renderer* renderer, u32 x, u32 y, float r, float g, float b);
void text_renderer_set_character_background_color(text_renderer* renderer, u32 x, u32 y, float r, float g, float b, bool paint);

u32 text_renderer_width(text_renderer* renderer);
u32 text_renderer_height(text_renderer* renderer);
u32 text_renderer_window_width(text_renderer* renderer);
u32 text_renderer_window_height(text_renderer* renderer);
u32 text_renderer_character_size(text_renderer* renderer);

void text_renderer_get_mouse_grid_position(window_handle* window, text_renderer* renderer, u32* x, u32* y);