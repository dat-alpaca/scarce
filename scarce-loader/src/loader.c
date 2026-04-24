#include "loader.h"

void* get_application_space(const char* filepath, u64 memoryPageAmount)
{
    file_descriptor mainBinary = platform_open_file(filepath, SCA_FILE_READ);
    if (mainBinary == invalid_file_descriptor)
        return NULL;

    void* applicationSpace = platform_mmap
    (
        NULL,
        TO_KiB(2),
        PROTECTION_READ | PROTECTION_WRITE | PROTECTION_EXECUTE, 
        MEMORY_PRIVATE | MEMORY_ANON, 
        invalid_file_descriptor, 0
    );

    platform_read_file(mainBinary, applicationSpace, TO_KiB(4) * memoryPageAmount);
    return applicationSpace;
}

void get_exported_functions(void* applicationSpace, load_func* onLoad, update_func* onUpdate, unload_func* onUnload)
{
    exports* e = (exports*)applicationSpace;
    
    *onLoad   = (load_func)((u8*)applicationSpace + (u64)e->on_load);
    *onUpdate = (update_func)((u8*)applicationSpace + (u64)e->on_update);
    *onUnload = (unload_func)((u8*)applicationSpace + (u64)e->on_unload);
}