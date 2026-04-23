#include <fcntl.h>
#include <unistd.h>

#include "platform/platform.h"

file_descriptor platform_open_file(const char* filepath, file_mode modeFlags)
{
    int flags = 0;
    if (modeFlags & SCA_FILE_READ && !(modeFlags & SCA_FILE_WRITE))
        flags |= O_RDONLY;
    else if (modeFlags & SCA_FILE_WRITE && !(modeFlags & SCA_FILE_READ))
        flags |= O_WRONLY;
    else
        flags |= O_RDWR;

    if (modeFlags & SCA_FILE_CREATE)
    {
        flags |= O_CREAT;
        return open(filepath, flags, 0644); // S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
    }

    return open(filepath, flags);
}

void platform_read_file(file_descriptor fileDescriptor, void* buffer, u32 length)
{
    read(fileDescriptor, buffer, length);
}

void platform_write_file(file_descriptor fileDescriptor, void* buffer, u32 length)
{
    write(fileDescriptor, buffer, length);
}

bool platform_is_file_open(file_descriptor fileDescriptor)
{
    return (fileDescriptor != invalid_file_descriptor);
}

u32 platform_file_size(file_descriptor fileDescriptor)
{
    if (fileDescriptor == invalid_file_descriptor)
        return 0;

    u32 size = lseek(fileDescriptor, 0, SEEK_END);
    lseek(fileDescriptor, 0, SEEK_SET);

    return size;
}

void platform_close_file(file_descriptor fileDescriptor)
{
    if (!platform_is_file_open(fileDescriptor))
        return;

    close(fileDescriptor);
}
