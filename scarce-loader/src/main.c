#include "assets/asset_library.h"
#include "assets/asset_upload.h"

#include "logging/logger.h"
#include "random.h"
#include "rhi/rhi.h"
#include "scarce.h"

#include "graphics/batch_renderer.h"
#include "config/loader.h"
#include "loader.h"

#include "platform/platform.h"
#include "texture.h"
#include "ui/hsml/hsml.h"
#include "ui/ui.h"
#include "view/view.h"

static engine gEngine =
{
    // AABB:
    .aabb_check_collision = aabb_check_collision,

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

    // View:
    .viewHolder = NULL,
    .view_holder_init = view_holder_init,
    .view_holder_destroy = view_holder_destroy,
    .view_holder_register = view_holder_register,

    .view_holder_update = view_holder_update,
    .view_holder_render = view_holder_render,

    .view_holder_switch_view = view_holder_switch_view,
    .view_holder_current = view_holder_current,

    // Rendering:
    .renderer_set_scale = batch_renderer_set_scale,
    .renderer_zero_buffer = batch_renderer_zero_buffer,
    .renderer_get_cell = batch_renderer_get_cell,
    .renderer_set_cell = batch_renderer_set_cell,
    .renderer_get_mouse_grid_position = batch_renderer_get_mouse_grid_position,

    // Logging:
    .log_info = log_info,
    .log_warn = log_warn,
    .log_error = log_error,
    .log_critical = log_critical,
    .logger_set_header = logger_set_header,

    // UI:
    .ui_begin = ui_begin,
    .ui_begin_stack = ui_begin_stack,
    .ui_end = ui_end,
    .ui_clear = ui_clear,

    .ui_text = ui_text,
    .ui_number = ui_number,

    .ui_set_align = ui_set_align,
    .ui_set_position = ui_set_position,
    .ui_set_color = ui_set_color,

    .ui_sameline = ui_sameline,
    .ui_feed = ui_feed,
    .ui_nudge = ui_nudge,
    .ui_space = ui_space,
    .ui_hline = ui_hline,
    .ui_vline = ui_vline,

    .ui_mouse_aabb = ui_mouse_aabb,
    
    // UI Button:
    .ui_button_init = ui_button_init,
    .ui_button_render = ui_button_render,
    .ui_button_update = ui_button_update,

    .ui_button_aabb = ui_button_aabb,

    // UI Textbox:
    .ui_text_box_init = ui_text_box_init,
    .ui_text_box_render = ui_text_box_render,
    .ui_text_box_update = ui_text_box_update,

    .ui_text_box_aabb = ui_text_box_aabb,

    // UI HSML:
    ///.ui_hsml = ui_hsml,
    .ui_hsml = ui_hsml,
};

struct context
{
    void* applicationSpace;
    u8* memoryPool;
    asset_library assetLibrary;
    batch_renderer textRenderer;
    view_holder viewHolder;
    rhi rhi;
} typedef context;

/* Window */
void window_size_callback(window_handle* window, i32 width, i32 height)
{
    context* userContext = (context*)window_get_user_pointer(window);
    batch_renderer* r = &userContext->textRenderer;

    rhi_set_viewport(userContext->rhi, 0, 0, width, height);
    batch_renderer_on_resize(r, width, height);
}

static bool initialize(context* context, config* config)
{
    random_init();

    // User space:
    context->applicationSpace = get_application_space(config->mainBinaryFilepath, config->memoryPageAmount);
    if (!context->applicationSpace)
        return false;
    gEngine.baseAddress = context->applicationSpace;

    context->memoryPool = (u8*)malloc(config->userSpaceBytes);
    gEngine.memoryPoolSize = config->userSpaceBytes;

    // Window:
    gEngine.window = window_init(config->windowTitle, config->minWindowWidth, config->minWindowHeight);
    if (!gEngine.window)
        return false;

    window_set_user_pointer(gEngine.window, context);
    window_set_resize_callback(gEngine.window, window_size_callback);

    // RHI:
    context->rhi = rhi_init();
    rhi_initialize_window(context->rhi, gEngine.window);

    // Assets:
    asset_library_init(&context->assetLibrary, 2);
    asset_handle mainFont = asset_library_load_font(&context->assetLibrary, config->fontFilepath, 64);
    asset_handle spriteHandle = asset_library_load_spritesheet(&context->assetLibrary, "assets/core/items.png", 16);
    spritesheet* fontSpritesheet = asset_library_get_spritesheet(&context->assetLibrary, spriteHandle);
    texture_handle fontTexture = upload_font_spritesheets(context->rhi, fontSpritesheet);

    // View:
    gEngine.viewHolder = &context->viewHolder;
    view_holder_init(gEngine.viewHolder, 3);

    // Logger:
    logger mainLogger;
    logger_initialize(&mainLogger);
    gEngine.logger = &mainLogger;

    // Font Renderer:
    shader_filepaths shaders = { .vertexFilepath = config->vertexFilepath, .fragmentFilepath = config->fragmentFilepath };
    batch_renderer_init(&context->textRenderer, context->rhi, gEngine.window, &shaders, config->minWindowWidth, config->minWindowHeight, 24);
    batch_renderer_set_texture(&context->textRenderer, fontTexture);
    gEngine.renderer = &context->textRenderer;

    return true;
} 

int main()
{   
    context context = { 0 };
    config_result configResult = load_config("config.ini");
    if (!configResult.sucess)
        return 1;

    config* config = &configResult.configuration;
    if(!initialize(&context, config))
        return 1;
        
    load_func onLoad;
    update_func onUpdate;
    get_exported_functions(context.applicationSpace, &onLoad, &onUpdate);

    gEngine.requestExit = false;
    onLoad(context.memoryPool, &gEngine);
    while(window_is_open(gEngine.window))
    {
        window_poll_events(gEngine.window);

        if(!onUpdate(context.memoryPool) || gEngine.requestExit)
            break;

        batch_renderer_render(&context.textRenderer);

        rhi_swap_buffers(context.rhi, gEngine.window);
    }
    
    return 0;
}