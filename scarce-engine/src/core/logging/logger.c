#include "logger.h"
#include "platform/platform.h"
#include "core/fixed_array.h"

void logger_initialize(logger* logger)
{
    logger->printHeader = true;
}
void logger_set_header(logger* logger, bool headerState)
{
    logger->printHeader = headerState;
}

void log_warn(logger* logger, const char* message, u32 length)
{
    if(logger->printHeader)
    {
        const char* prefix = "[" SCA_YELLOW "Warn" SCA_RESET "]: ";
        const u32 prefixLength = 18;

        platform_write_file(platform_stderr(), (char*)prefix, prefixLength);
    }

    fixed_array buffer = { 0 };
    fixed_array_init(&buffer, length);
    fixed_array_push(&buffer, (void*)message, length);

    platform_write_file(platform_stderr(), buffer.buffer, length);
    platform_write_file(platform_stderr(), "\n", 1);

    fixed_array_destroy(&buffer);
}

void log_error(logger* logger, const char* message, u32 length)
{
    if(logger->printHeader)
    {
        const char* prefix = "[" SCA_RED "Error" SCA_RESET "]: ";
        const u32 prefixLength = 19;

        platform_write_file(platform_stderr(), (char*)prefix, prefixLength);
    }

    fixed_array buffer = { 0 };
    fixed_array_init(&buffer, length);
    fixed_array_push(&buffer, (void*)message, length);

    platform_write_file(platform_stderr(), buffer.buffer, length);
    platform_write_file(platform_stderr(), "\n", 1);

    fixed_array_destroy(&buffer);
}

void log_info(logger* logger, const char* message, u32 length)
{
    if(logger->printHeader)
    {
        const char* prefix = "[" SCA_BLUE "Info" SCA_RESET "]: ";
        const u32 prefixLength = 18;

        platform_write_file(platform_stderr(), (char*)prefix, prefixLength);
    }

    fixed_array buffer = { 0 };
    fixed_array_init(&buffer, length);
    fixed_array_push(&buffer, (void*)message, length);

    platform_write_file(platform_stderr(), buffer.buffer, length);
    platform_write_file(platform_stderr(), "\n", 1);

    fixed_array_destroy(&buffer);
}

void log_critical(logger* logger, const char* message, u32 length)
{
    if(logger->printHeader)
    {
        const char* prefix = "[" SCA_MAGENTA "Critical" SCA_RESET "]: ";
        const u32 prefixLength = 22;

        platform_write_file(platform_stderr(), (char*)prefix, prefixLength);
    }

    fixed_array buffer = { 0 };
    fixed_array_init(&buffer, length);
    fixed_array_push(&buffer, (void*)message, length);

    platform_write_file(platform_stderr(), buffer.buffer, length);
    platform_write_file(platform_stderr(), "\n", 1);

    fixed_array_destroy(&buffer);
    platform_breakpoint();
}