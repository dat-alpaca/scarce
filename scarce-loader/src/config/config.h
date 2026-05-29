#pragma once
#include <stdbool.h>
#include "core/defines.h"

typedef struct config
{
    const char* windowTitle;
    const char* mainBinaryFilepath;

    const char* vertexFilepath;
    const char* fragmentFilepath;
    const char* fontFilepath;
    
    u64 memoryPageAmount;
    u64 userSpaceBytes;
    u32 minWindowWidth;
    u32 minWindowHeight;

    u64 unknownMemoryCapacity;
    u64 transientMemoryCapacity;
    u64 generalMemoryCapacity;
} config;

typedef struct
{
    config configuration;
    bool sucess;
} config_result;

config_result load_config(const char* configFilepath);