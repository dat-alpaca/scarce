#include "shader.h"
#include "fixed_array.h"
#include "platform/platform.h"

gl_handle read_shader(const char* filepath, shader_type type)
{
    file_descriptor file = platform_open_file(filepath, SCA_FILE_READ | SCA_FILE_CREATE);
    if (file == invalid_file_descriptor)
        return -1;

    u32 fileSize = platform_file_size(file);
    if (fileSize <= 0)
        return -1;
    
    fixed_array buffer = { 0 };
    fixed_array_init(&buffer, fileSize + 1);

    platform_read_file(file, buffer.buffer, fileSize);
    buffer.current = fileSize;

    char null = '\0';
    fixed_array_push(&buffer, &null, 1);

    gl_handle shader = graphics_create_shader(type, buffer.buffer);
    platform_close_file(file);
    fixed_array_destroy(&buffer);

    return shader;
}