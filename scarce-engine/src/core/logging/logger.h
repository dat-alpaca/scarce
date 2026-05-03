#pragma once
#include "core/defines.h"

void logger_initialize();

void log_warn(const char* message, u32 length);
void log_error(const char* message, u32 length);
void log_info(const char* message, u32 length);
void log_critical(const char* message, u32 length);