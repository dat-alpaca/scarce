#include <io.h>
#include <fcntl.h>

#include "platform/platform.h"

file_descriptor platform_open_file(const char* filepath, file_mode modeFlags)
{
    int flags = 0;
    if (modeFlags & SCA_FILE_READ && !(modeFlags & SCA_FILE_WRITE))
        flags |= _O_RDONLY;
    else if (modeFlags & SCA_FILE_WRITE && !(modeFlags & SCA_FILE_READ))
        flags |= _O_WRONLY;
    else
        flags |= _O_RDWR;

    if (modeFlags & SCA_FILE_CREATE)
        flags |= _O_CREAT;

    return _open(filepath, flags);
}
bool platform_is_file_open(file_descriptor fileDescriptor)
{
    return (fileDescriptor != -1);
}
void platform_close_file(file_descriptor fileDescriptor)
{
    if (!platform_is_file_open(fileDescriptor))
        return;
    
    _close(fileDescriptor);
}