#include "core/font/font.h"
#include "freetype/freetype.h"
#include "graphics/graphics.h"
#include "logging/logger.h"
#include "random.h"
#include "scarce.h"

#include "config/loader.h"
#include "graphics/text_renderer.h"
#include "loader.h"

#include "platform/platform.h"

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

    // Random:
    .random_int = random_int,
    .random_float = random_float,

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
    .is_mouse_btn_pressed = window_is_mouse_btn_pressed,

    // Rendering:
    .renderer_zero_buffer = text_renderer_zero_buffer,
    .renderer_set_character_letter = text_renderer_set_character_letter,
    .renderer_set_character_color = text_renderer_set_character_color,
    .renderer_set_character_background_color = text_renderer_set_character_background_color,
    .renderer_set_character_size = text_renderer_set_character_size,
    .renderer_width = text_renderer_width,
    .renderer_height = text_renderer_height,
    .renderer_window_width = text_renderer_window_height,
    .renderer_window_height = text_renderer_window_height,
    .renderer_character_size = text_renderer_character_size,
    
    .get_mouse_position = text_renderer_get_mouse_grid_position,

    // Logging:
    .log_info = log_info,
    .log_warn = log_warn,
    .log_error = log_error,
    .log_critical = log_critical,
    .logger_set_header = logger_set_header,
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

struct
{
    text_renderer renderer;
    FT_Library library;
} typedef context;

/* Window */
void window_size_callback(window_handle* window, i32 width, i32 height)
{
    context* userContext = (context*)window_get_user_pointer(window);
    text_renderer* r = &userContext->renderer;

    glViewport(0, 0, width, height);
    text_renderer_on_resize(r, width, height);
}

int main()
{   
    config_result configResult = load_config("config.ini");
    if (!configResult.sucess)
        return 1;

    config* config = &configResult.configuration;

    context context = { 0 };
    u8* memoryPool = (u8*)malloc(config->userSpaceBytes);
    gEngine.memoryPoolSize = config->userSpaceBytes;

    random_init();
    font_loader_init(&context.library);

    // Application space:
    load_func onLoad;
    update_func onUpdate;
    unload_func onUnload;
    void* applicationSpace = get_application_space(config->mainBinaryFilepath, config->memoryPageAmount);
    if (!applicationSpace)
        return 1;

    get_exported_functions(applicationSpace, &onLoad, &onUpdate, &onUnload);

    // Window:
    u32 characterSize = 16;
    u32 screenX = 640;
    u32 screenY = 480;
    gEngine.window = window_init("Scarce v0.1", screenX, screenY);
    if (!gEngine.window)
        return 1;

    window_set_user_pointer(gEngine.window, &context);
    window_set_resize_callback(gEngine.window, window_size_callback);

    // Application resources:
    gl_handle fontTexture;
    font monoFont;
    initialize_data(&context.library, config->fontFilepath, &fontTexture, &monoFont);
    
    // Renderer:
    shader_filepaths shaders = { .vertexFilepath = config->vertexFilepath, .fragmentFilepath = config->fragmentFilepath };
    text_renderer_init(&context.renderer, &monoFont, &shaders, screenX, screenY, characterSize);
    text_renderer_set_texture(&context.renderer, fontTexture);
    gEngine.renderer = &context.renderer;

    // Logger:
    logger mainLogger;
    logger_initialize(&mainLogger);
    gEngine.logger = &mainLogger;

    onLoad(memoryPool, &gEngine);
    while(window_is_open(gEngine.window))
    {
        window_poll_events(gEngine.window);

        if(!onUpdate(memoryPool))
            break;

        text_renderer_render(&context.renderer);

        window_swap_buffers(gEngine.window);
    }
    onUnload(memoryPool);
    
    return 0;
}