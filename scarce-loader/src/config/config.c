#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ini.h>

#include "config.h"
#include "defines.h"
#include "string_utils.h"
#include "platform/platform.h"

static int handler(void* user, const char* section, const char* name, const char* value)
{
    config* pConfig = (config*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    // General:
    if (MATCH("general", "spritesheetCapacity"))
        pConfig->spritesheetCapacity = atoi(value);
    
    else if (MATCH("general", "viewCapacity"))
        pConfig->viewCapacity = atoi(value);

    // Window:
    else if (MATCH("window", "windowTitle")) 
        pConfig->windowTitle = strdup(value);

    else if (MATCH("window", "minWindowWidth"))
        pConfig->minWindowWidth = atoi(value);

    else if (MATCH("window", "minWindowHeight"))
        pConfig->minWindowHeight = atoi(value);

    // User space:
    else if (MATCH("user", "mainBinaryFilepath")) 
        pConfig->mainBinaryFilepath = strdup(value);
    
    else if (MATCH("user", "memoryPageAmount"))
        pConfig->memoryPageAmount = atoi(value);

    else if (MATCH("user", "userSpaceBytes"))
        pConfig->userSpaceBytes = atoi(value);

    // Text renderer:
    else if (MATCH("text_renderer", "fontFilepath"))
        pConfig->fontFilepath = strdup(value);

    else if (MATCH("text_renderer", "vertexFilepath"))
        pConfig->vertexFilepath = strdup(value);

    else if (MATCH("text_renderer", "fragmentFilepath"))
        pConfig->fragmentFilepath = strdup(value);
    
    else if (MATCH("text_renderer", "mainFontHeight"))
        pConfig->mainFontHeight = atoi(value);

    // Memory:
    else if (MATCH("memory", "unknownMemoryCapacity"))
        pConfig->unknownMemoryCapacity = atoll(value);
    
    else if (MATCH("memory", "assetsMemoryCapacity"))
        pConfig->assetsMemoryCapacity = atoll(value);
    
    else if (MATCH("memory", "rendererMemoryCapacity"))
        pConfig->rendererMemoryCapacity = atoll(value);

    else if (MATCH("memory", "generalMemoryCapacity"))
        pConfig->generalMemoryCapacity = atoll(value);

    else if (MATCH("memory", "hsmlMemoryCapacity"))
        pConfig->hsmlMemoryCapacity = atoll(value);

    else if (MATCH("memory", "transientMemoryCapacity"))
        pConfig->transientMemoryCapacity = atoll(value);

    else
        return 0;
    
    return 1;
}

static config_result create_default_config(const char* configFilepath)
{
    config_result result = { 0 };

    file_descriptor file = platform_open_file(configFilepath, SCA_FILE_CREATE | SCA_FILE_WRITE | SCA_FILE_READ);
    if (file == invalid_file_descriptor)
        return result;

    config config = {
        .spritesheetCapacity = 2,
        .viewCapacity = 2,
        
        .windowTitle = "Scarce",
        .minWindowWidth = 640,
        .minWindowHeight = 480,

        .mainBinaryFilepath = "main.bin",
        .memoryPageAmount = 1,
        .userSpaceBytes = 1024,

        .fontFilepath = "assets/core/basis33/basis33.ttf",
        .vertexFilepath = "assets/core/shaders/text.vert",
        .fragmentFilepath = "assets/core/shaders/text.frag",
        .mainFontHeight = 64,

        .unknownMemoryCapacity = 8,
        .assetsMemoryCapacity = TO_KiB(2),
        .rendererMemoryCapacity = TO_KiB(32),
        .generalMemoryCapacity = TO_KiB(10),
        .hsmlMemoryCapacity = 256,
        .transientMemoryCapacity = 2560,
    };

    char buffer[1024];
    int length = snprintf(buffer, sizeof(buffer),
        "[general]\n"
        "spritesheetCapacity = %" PRIu32 "\n" 
        "viewCapacity = %" PRIu32 "\n" 
        "\n"
        "[window]\n"
        "windowTitle = %s\n"
        "minWindowWidth = %" PRIu32 "\n"
        "minWindowHeight = %" PRIu32 "\n" 
        "\n"
        "[user]\n"
        "mainBinaryFilepath = %s\n"
        "memoryPageAmount = %" PRIu64 "\n"
        "userSpaceBytes = %" PRIu64 "\n"
        "\n"
        "[text_renderer]\n"
        "fontFilepath = %s\n"
        "vertexFilepath = %s\n"
        "fragmentFilepath = %s\n"
        "mainFontHeight = %" PRIu32 "\n" 
        "\n"
        "[memory]\n"
        "unknownMemoryCapacity = %" PRIu64 "\n"
        "assetsMemoryCapacity = %" PRIu64 "\n"
        "rendererMemoryCapacity = %" PRIu64 "\n"
        "generalMemoryCapacity = %" PRIu64 "\n"
        "hsmlMemoryCapacity = %" PRIu64 "\n"
        "transientMemoryCapacity = %" PRIu64 "\n"
        ,
        config.spritesheetCapacity,
        config.viewCapacity,

        config.windowTitle,
        config.minWindowWidth,
        config.minWindowHeight,

        config.mainBinaryFilepath,
        config.memoryPageAmount,
        config.userSpaceBytes,

        config.fontFilepath,
        config.vertexFilepath,
        config.fragmentFilepath,
        config.mainFontHeight,

        config.unknownMemoryCapacity,
        config.assetsMemoryCapacity,
        config.rendererMemoryCapacity,
        config.generalMemoryCapacity,
        config.hsmlMemoryCapacity,
        config.transientMemoryCapacity
    );
    if (length > 0)
        platform_write_file(file, (void*)buffer, (u32)length);
    else
        return result;

    platform_close_file(file);
    result.configuration = config;
    result.sucess = true;
    return result;
}

config_result load_config(const char* configFilepath)
{
    config_result result = { 0 };
    
    if (ini_parse(configFilepath, handler, &result) < 0) 
        return create_default_config(configFilepath);

    result.sucess = true;
    return result;
}