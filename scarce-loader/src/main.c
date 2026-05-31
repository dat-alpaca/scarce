#include "assets/asset_library.h"
#include "assets/asset_upload.h"

#include "memory/memory_system.h"
#include "random.h"
#include "scarce.h"

#include "config/config.h"
#include "loader.h"

extern engine gEngine;

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
void window_size_callback(window_handle window, i32 width, i32 height)
{
    context* userContext = (context*)window_get_user_pointer(window);
    batch_renderer* r = &userContext->textRenderer;

    rhi_set_viewport(userContext->rhi, 0, 0, width, height);
    batch_renderer_on_resize(r, width, height);
}

static bool initialize(context* context, config* config)
{
    random_init();

    // Memory:
    memory_options options;
    {
        options.memoryAmountPerTag[TAG_UNKNOWN] = config->unknownMemoryCapacity;
        options.memoryAmountPerTag[TAG_SYSTEM] = TO_KiB(0.5);
        options.memoryAmountPerTag[TAG_USER] = config->userSpaceBytes;

        options.memoryAmountPerTag[TAG_ASSETS] = config->assetsMemoryCapacity;
        options.memoryAmountPerTag[TAG_RENDERER] = config->rendererMemoryCapacity;
        options.memoryAmountPerTag[TAG_GENERAL] = config->generalMemoryCapacity;
        options.memoryAmountPerTag[TAG_HSML] = config->hsmlMemoryCapacity;
        
        options.memoryAmountPerTag[TAG_TRANSIENT] = config->transientMemoryCapacity;
    }
    memory_system_init(&options);

    // User space:
    context->applicationSpace = get_application_space(config->mainBinaryFilepath, config->memoryPageAmount);
    if (!context->applicationSpace)
        return false;
    gEngine.baseAddress = context->applicationSpace;

    context->memoryPool = sca_allocate(TAG_USER, NULL, config->userSpaceBytes, 1);
    gEngine.memoryPoolSize = config->userSpaceBytes;

    // Window & RHI:
    context->rhi = rhi_init();
    gEngine.window = window_init();
    if (!gEngine.window || !context->rhi)
        return false;

    rhi_preinitialize_window(context->rhi, gEngine.window);
    window_create(gEngine.window, config->windowTitle, config->minWindowWidth, config->minWindowHeight);
    
    window_set_user_pointer(gEngine.window, context);
    window_set_resize_callback(gEngine.window, window_size_callback);

    rhi_initialize_window(context->rhi, gEngine.window);

    // Assets:
    asset_library_init(&context->assetLibrary, config->spritesheetCapacity);
    asset_handle mainFont = asset_library_load_font(&context->assetLibrary, config->fontFilepath, config->mainFontHeight);
    //asset_handle spriteHandle = asset_library_load_spritesheet(&context->assetLibrary, "assets/core/items.png", 16);
    spritesheet* fontSpritesheet = asset_library_get_spritesheet(&context->assetLibrary, mainFont);
    texture_handle fontTexture = upload_font_spritesheets(context->rhi, fontSpritesheet);

    // View:
    gEngine.viewHolder = &context->viewHolder;
    view_holder_init(gEngine.viewHolder, config->viewCapacity);

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

static void __debug_memory_usage()
{
    printf("----------\n");

    u64 capacity, used;
    for (memory_tag tag = TAG_START; tag < TAG_AMOUNT; ++tag)
    {
        memory_status status = memory_system_status(tag);
        capacity = status.memoryCapacity;
        used = status.memoryUsed;

        const char* name = get_system_memory_tag_name(tag);
        u32 nameLength = strlen(name);
        u32 leftWidth = nameLength + (11 - nameLength) / 2; 

        printf("[%*s%*s]: %7ld / %7ld bytes", 
            leftWidth, name,
            11 - leftWidth, "", 
            used, 
            capacity
        );

        if (used > capacity)
            printf(" [" SCA_RED "!" SCA_RESET "]");

        printf("\n");    
    }
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

        // __debug_memory_usage();

        if(!onUpdate(context.memoryPool) || gEngine.requestExit)
            break;

        batch_renderer_render(&context.textRenderer);

        rhi_swap_buffers(context.rhi, gEngine.window);
    }
    
    return 0;
}