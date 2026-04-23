#include "memory.h"

void* scarce_read(u8* memoryPool, u32 address)
{
    return &(memoryPool[address]);
}
void scarce_write(u8* memoryPool, u32 address, u8 value)
{
    memoryPool[address] = value;
}
void scarce_write_u32(memory_pool* memoryPool, u32 address, u32 value)
{
    uint8_t b1 = (value >> 24) & 0xFF;
    uint8_t b2 = (value >> 16) & 0xFF;
    uint8_t b3 = (value >>  8) & 0xFF;
    uint8_t b4 = (value      ) & 0xFF;
    
    scarce_write(memoryPool, address, b1);
    scarce_write(memoryPool, address, b2);
    scarce_write(memoryPool, address, b3);
    scarce_write(memoryPool, address, b4);
}
void scarce_copy(memory_pool* memoryPool, u32 address, void* buffer, u8 size)
{
    // I hope you know what you're doing.

    for(u64 i = 0; i < size; ++i)
    {
        u8* data = (u8*)buffer;
        memoryPool[address + i] = data[i];
    }
}
