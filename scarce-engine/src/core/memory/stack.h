#pragma once
#include "memory.h"

typedef struct
{
    u8 size;
} stack;

typedef void* (*scarce_push_func)(memory_pool* memoryPool, u16 size);
typedef void (*scarce_pop_func)(memory_pool* memoryPool, u16 size);

void* scarce_push(memory_pool* memoryPool, u16 size);
void scarce_pop(memory_pool* memoryPool, u16 size);

#define SCA_READ_STACK(pool) (pool[SCA_STACK_ADDRESS])