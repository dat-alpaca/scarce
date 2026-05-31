#pragma once
#include "memory.h"

typedef struct
{
    u8 size;
} stack;

void* scarce_push(memory_pool* memoryPool, u16 size);
void scarce_pop(memory_pool* memoryPool, u16 size);

#define SCA_READ_STACK(pool) (pool[SCA_STACK_ADDRESS])