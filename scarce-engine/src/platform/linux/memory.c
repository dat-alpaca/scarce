#include "platform/platform.h"
#include <sys/mman.h>
#include <unistd.h>

void* platform_mmap(void *address, u32 length, protection_mode protection, memory_flags flags, file_descriptor fileDescriptor, i64 offset)
{
    i32 protectionFlags = 0;
    i32 memoryFlags = 0;

    // Protection:
    if (protection & PROTECTION_READ)
        protectionFlags |= PROT_READ;

    if (protection & PROTECTION_WRITE)
        protectionFlags |= PROT_WRITE;

    if (protection & PROTECTION_EXECUTE)
        protectionFlags |= PROT_EXEC;

    // Memory:
    if (flags & MEMORY_PRIVATE)
        memoryFlags |= MAP_PRIVATE;
    
    if (flags & MEMORY_SHARED)
        memoryFlags |= MAP_SHARED;

    if (flags & MEMORY_ANON)
        memoryFlags |= MAP_ANONYMOUS;

    return mmap(address, length, protectionFlags, memoryFlags, fileDescriptor, offset);
}

void platform_munmmap(void *address, u32 length)
{
    munmap(address, length);
}

u64 platform_page_size()
{
    return (u64)sysconf(_SC_PAGESIZE);
}