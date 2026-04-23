#include "utils.h"
#include "map.h"

const u8 get_stack_remaining_size(memory_pool* pool)
{
    if (pool[SCA_STACK_SIZE_ADDRESS] >= SCA_STACK_SIZE - 1)
        return 0;

    return SCA_STACK_SIZE - pool[SCA_STACK_SIZE_ADDRESS] - 1;
}

u8 open_file(engine* e, memory_pool* pool, const char* filename, file_mode mode)
{
    file_descriptor* baseDescriptor = (file_descriptor*)&pool[SCA_FILE_DESCRIPTORS_ADDR];
    u8 foundAddress = invalid_handle;
    for (u8 i = 0; i < SCA_FILE_DESCRIPTORS_SIZE / sizeof(file_descriptor); ++i)
    {
        if (baseDescriptor[i] == invalid_file_descriptor)
        {
            foundAddress = i;
            break;
        }
    }

    if (foundAddress != invalid_handle)
    {
        baseDescriptor[foundAddress] = e->platform_open_file(filename, mode); 
        if (baseDescriptor[foundAddress] == invalid_file_descriptor)
            return invalid_handle;
    }
    
    return foundAddress;
}