#pragma once
#include <stdbool.h>

typedef struct
{
    const char* mainBinaryFilepath;
    const char* vertexFilepath;
    const char* fragmentFilepath;
    const char* fontFilepath;
} config;

typedef struct
{
    config configuration;
    bool sucess;
} config_result;

config_result load_config(const char* configFilepath);