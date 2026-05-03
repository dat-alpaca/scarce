#include "logger.h"
#include "platform/platform.h"
#include "core/fixed_array.h"
#include <complex.h>

void log_warn(const char* message, u32 length)
{
    const char* prefix = "[" SCA_YELLOW "Warn" SCA_RESET "]: ";
    const u32 prefixLength = 18;

    fixed_array buffer = { 0 };
    fixed_array_init(&buffer, length + prefixLength);
    fixed_array_push(&buffer, (void*)prefix, prefixLength);
    fixed_array_push(&buffer, (void*)message, length);

    platform_write_file(platform_stderr(), buffer.buffer, length + prefixLength);
    platform_write_file(platform_stderr(), "\n", 1);

    fixed_array_destroy(&buffer);
}

void log_error(const char* message, u32 length)
{
    const char* prefix = "[" SCA_RED "Error" SCA_RESET "]: ";
    const u32 prefixLength = 19;

    fixed_array buffer = { 0 };
    fixed_array_init(&buffer, length + prefixLength);
    fixed_array_push(&buffer, (void*)prefix, prefixLength);
    fixed_array_push(&buffer, (void*)message, length);

    platform_write_file(platform_stderr(), buffer.buffer, length + prefixLength);
    platform_write_file(platform_stderr(), "\n", 1);

    fixed_array_destroy(&buffer);
}

void log_info(const char* message, u32 length)
{
    const char* prefix = "[" SCA_BLUE "Info" SCA_RESET "]: ";
    const u32 prefixLength = 18;

    fixed_array buffer = { 0 };
    fixed_array_init(&buffer, length + prefixLength);
    fixed_array_push(&buffer, (void*)prefix, prefixLength);
    fixed_array_push(&buffer, (void*)message, length);

    platform_write_file(platform_stderr(), buffer.buffer, length + prefixLength);
    platform_write_file(platform_stderr(), "\n", 1);

    fixed_array_destroy(&buffer);
}

void log_critical(const char* message, u32 length)
{
    const char* prefix = "[" SCA_MAGENTA "Critical" SCA_RESET "]: ";
    const u32 prefixLength = 22;

    fixed_array buffer = { 0 };
    fixed_array_init(&buffer, length + prefixLength);
    fixed_array_push(&buffer, (void*)prefix, prefixLength);
    fixed_array_push(&buffer, (void*)message, length);

    platform_write_file(platform_stderr(), buffer.buffer, length + prefixLength);
    platform_write_file(platform_stderr(), "\n", 1);

    fixed_array_destroy(&buffer);

    platform_breakpoint();
}