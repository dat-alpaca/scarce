#pragma once
#include <stdbool.h>
#include "core/defines.h"

typedef struct config
{
    // General:
    u32 spritesheetCapacity;
    u32 viewCapacity;

    // Window:
    const char* windowTitle;
    u32 minWindowWidth;
    u32 minWindowHeight;

    // User space:
    const char* mainBinaryFilepath;
    u64 memoryPageAmount;
    u64 userSpaceBytes;

    // Text renderer:
    const char* fontFilepath;
    const char* vertexFilepath;
    const char* fragmentFilepath;
    u32 mainFontHeight;
    
    // Memory arenas:
    u64 unknownMemoryCapacity;
    u64 transientMemoryCapacity;
    u64 generalMemoryCapacity;
    u64 hsmlMemoryCapacity;
    u64 assetsMemoryCapacity;
    u64 rendererMemoryCapacity;
} config;

typedef struct
{
    config configuration;
    bool sucess;
} config_result;

config_result load_config(const char* configFilepath);