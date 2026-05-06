#pragma once
#include "core/defines.h"

typedef struct
{
    bool printHeader;
} logger;

void logger_initialize(logger* logger);
void logger_set_header(logger* logger, bool headerState);

void log_warn(logger* logger, const char* message, u32 length);
void log_error(logger* logger, const char* message, u32 length);
void log_info(logger* logger, const char* message, u32 length);
void log_critical(logger* logger, const char* message, u32 length);