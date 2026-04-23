#include "core/font/font.h"
#include "freetype/freetype.h"
#include "graphics/graphics.h"
#include "scarce.h"

#include "config/loader.h"
#include "graphics/text_renderer.h"
#include "core/window/window.h"
#include "loader.h"
#include <string.h>

#define GLEW_NO_GLU
#include <GL/glew.h>

static engine gEngine =
{
    // Memory:
    .scarce_read = scarce_read,
    .scarce_write = scarce_write,
    .scarce_write_u32 = scarce_write_u32,
    .scarce_copy = scarce_copy,

    .scarce_push = scarce_push,
    .scarce_pop = scarce_pop,

    // Timing:
    .get_timestamp_ns = platform_timestamp_ns,

    // File:
    .platform_open_file = platform_open_file,
    .platform_read_file = platform_read_file,
    .platform_write_file = platform_write_file,
    .platform_is_file_open = platform_is_file_open,
    .platform_file_size = platform_file_size,
    .platform_close_file = platform_close_file,

    // Input:
    .is_key_pressed = window_is_key_pressed,

    // Rendering:
    .renderer = NULL,
    .renderer_set_character_letter = text_renderer_set_character_letter,
    .renderer_set_character_color = text_renderer_set_character_color,
    .renderer_set_character_background_color = text_renderer_set_character_background_color,
    .renderer_set_character_size = text_renderer_set_character_size,
    .renderer_width = text_renderer_width,
    .renderer_height = text_renderer_height,
};

static void initialize_data(FT_Library* library, const char* fontFilepath, gl_handle* fontTexture, font* font)
{
    // Font:
    if(!load_font(library, font, fontFilepath))
        return;

    // Font texture:
    texture_information information = { 0 };
    {
        information.width = font->maxWidth * font->characters.current / sizeof(font_character);
		information.height = font->maxHeight;
	    information.layerWidth = font->maxWidth;
	    information.layerHeight = font->maxHeight;
		information.depth = 1;
		information.mipLevels = 1;
		information.samples = 1;
		information.arrayLayers = font->characters.current / sizeof(font_character);

		information.format = GL_R8;
		information.type = GL_TEXTURE_2D_ARRAY;

		information.generateMipmaps = false;

		information.wrapS = GL_REPEAT;
		information.wrapT = GL_REPEAT;
		information.minFilter = GL_NEAREST;
		information.magFilter = GL_NEAREST;
    }

    *fontTexture = graphics_create_texture(&information);

    font_character* characters = (font_character*)font->characters.buffer;
    u32 count = font->characters.current / sizeof(font_character);
    for(u32 i = 0; i < count; ++i)
	{
        font_character character = characters[i];

		if(!character.data.buffer)
			continue;

        vec2 offset;
        {
            offset[0] = 0;
            offset[1] = 0;
        }
        information.layerWidth = character.size[0];
        information.layerHeight = character.size[1];
        graphics_update_texture_array_layer(
            *fontTexture, &information, 
            character.layer, 
            offset, 
            GL_RED, 
            GL_UNSIGNED_BYTE, 
            character.data.buffer
        );
	}
}

const static double gResizeTimeout = 0.2;
struct
{
    text_renderer renderer;
    FT_Library library;
    double lastResize;
    bool isResizing;
} typedef context;

/* Window */
void window_size_callback(GLFWwindow* window, int width, int height)
{
    context* userContext = (context*)glfwGetWindowUserPointer(window);
    text_renderer* r = &userContext->renderer;

    glViewport(0, 0, width, height);
    
    if(!userContext->isResizing)
        text_renderer_on_resize(r, width, height);

    userContext->lastResize = glfwGetTime();
    userContext->isResizing = true;
}

int main()
{   
    config_result configResult = load_config("config.ini");
    if (!configResult.sucess)
        return 1;

    config* config = &configResult.configuration;

    context context = { 0 };
    static u8 memoryPool[TO_KiB(1) - 1];
    
    font_loader_init(&context.library);

    // Application space:
    load_func onLoad;
    update_func onUpdate;
    unload_func onUnload;
    void* applicationSpace = get_application_space(config->mainBinaryFilepath);
    if (!applicationSpace)
        return 1;

    get_exported_functions(applicationSpace, &onLoad, &onUpdate, &onUnload);

    // Window:
    u32 characterSize = 16;
    u32 screenX = 640;
    u32 screenY = 480;
    if (!window_init("Scarce v0.1", screenX, screenY))
        return 1;

    glfwSetWindowUserPointer(window, &context);
    glfwSetWindowSizeCallback(window, window_size_callback);

    // Application resources:
    gl_handle fontTexture;
    font monoFont;
    initialize_data(&context.library, config->fontFilepath, &fontTexture, &monoFont);
    
    // Renderer:
    shader_filepaths shaders = { .vertexFilepath = config->vertexFilepath, .fragmentFilepath = config->fragmentFilepath };
    text_renderer_init(&context.renderer, &monoFont, &shaders, screenX, screenY, characterSize);
    text_renderer_set_texture(&context.renderer, fontTexture);
    gEngine.renderer = &context.renderer;

    // World:
    mat4 project;
    glm_ortho(0.0f, (float)screenX, (float)screenY, 0.0f, 0.f, 1.0f, project);
    text_renderer_set_world(&context.renderer, project);

    onLoad(memoryPool, &gEngine);
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        if (context.isResizing && (glfwGetTime() - context.lastResize > gResizeTimeout)) 
        {
            context.isResizing = false;
        }

        if(!onUpdate(memoryPool))
            break;

        text_renderer_render(&context.renderer);

        glfwSwapBuffers(window);
    }
    onUnload(memoryPool);
    
    return 0;
}