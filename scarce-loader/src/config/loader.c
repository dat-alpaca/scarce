#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ini.h>

#include "loader.h"
#include "string_utils.h"
#include "platform/platform.h"

static int handler(void* user, const char* section, const char* name, const char* value)
{
    config* pConfig = (config*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("general", "mainBinaryFilepath")) 
        pConfig->mainBinaryFilepath = strdup(value);
    
    else if (MATCH("general", "vertexFilepath"))
        pConfig->vertexFilepath = strdup(value);

    else if (MATCH("general", "fragmentFilepath"))
        pConfig->fragmentFilepath = strdup(value);
    
    else if (MATCH("general", "fontFilepath"))
        pConfig->fontFilepath = strdup(value);

    else if (MATCH("general", "memoryPageAmount"))
        pConfig->memoryPageAmount = atoi(value);

    else if (MATCH("general", "userSpaceBytes"))
        pConfig->userSpaceBytes = atoi(value);

    else if (MATCH("general", "minWindowWidth"))
        pConfig->minWindowWidth = atoi(value);

    else if (MATCH("general", "minWindowHeight"))
        pConfig->minWindowHeight = atoi(value);

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
        .mainBinaryFilepath = "main.bin",
        .fontFilepath = "assets/core/basis33/basis33.ttf",
        .vertexFilepath = "assets/core/shaders/text.vert",
        .fragmentFilepath = "assets/core/shaders/text.frag",
        .memoryPageAmount = 1,
        .userSpaceBytes = 1024
    };

    char buffer[1024];
    int length = snprintf(buffer, sizeof(buffer),
        "[general]\n"
        "mainBinaryFilepath = %s\n"
        "fontFilepath = %s\n"
        "vertexFilepath = %s\n"
        "fragmentFilepath = %s\n"
        "memoryPageAmount = %" PRIu64 "\n"
        "userSpaceBytes = %" PRIu64 "\n"
        "minWindowWidth = %" PRIu32 "\n"
        "minWindowHeight = %" PRIu32 "\n",
        config.mainBinaryFilepath,
        config.fontFilepath,
        config.vertexFilepath,
        config.fragmentFilepath,
        config.memoryPageAmount,
        config.userSpaceBytes,
        config.minWindowWidth,
        config.minWindowHeight
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