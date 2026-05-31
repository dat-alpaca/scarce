#include "file_utils.h"

#include "fixed_array.h"
#include "logging/logger.h"
#include "memory/tag.h"
#include "platform/platform.h"

fixed_array file_read_contents(const char* filepath)
{
    file_descriptor file = platform_open_file(filepath, SCA_FILE_READ | SCA_FILE_CREATE);
    if (file == invalid_file_descriptor)
        log_critical_s("Failed to read file", 20);

    u32 fileSize = platform_file_size(file);
    if (fileSize <= 0)
        log_critical_s("Failed to read file bytes", 26);
    
    fixed_array buffer = { 0 };
    fixed_array_init(&buffer, fileSize + 1, sizeof(char), TAG_TRANSIENT);

    platform_read_file(file, buffer.buffer, fileSize);
    buffer.current = fileSize;

    char null = '\0';
    fixed_array_push(&buffer, &null, 1);

    platform_close_file(file);
    return buffer;
}