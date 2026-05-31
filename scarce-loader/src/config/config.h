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
    u64 hsmlMemoryCapacity;
    u64 assetsMemoryCapacity;
    u64 rendererMemoryCapacity;

    u32 spritesheetCapacity;
    u32 mainFontHeight;
} config;

typedef struct
{
    config configuration;
    bool sucess;
} config_result;

config_result load_config(const char* configFilepath);