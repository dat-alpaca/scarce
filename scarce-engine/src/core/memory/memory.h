#pragma once
#include "core/defines.h"

typedef u8 memory_pool;

void* scarce_read(memory_pool* memoryPool, u32 address);
void  scarce_write(memory_pool* memoryPool, u32 address, u8 value);
void  scarce_write_u32(memory_pool* memoryPool, u32 address, u32 value);
void  scarce_copy(memory_pool* memoryPool, u32 address, void* buffer, u8 size);

// Stack:
#define SCA_STACK_SIZE_ADDRESS     (0x0000)
#define SCA_STACK_ADDRESS          (0x0001)