#include "stack.h"
#include "memory/memory.h"

void* scarce_push(memory_pool* memoryPool, u16 size)
{
    memoryPool[SCA_STACK_SIZE_ADDRESS] += size;
    return memoryPool + SCA_STACK_ADDRESS + memoryPool[SCA_STACK_SIZE_ADDRESS] - size;
}
void scarce_pop(memory_pool* memoryPool, u16 size)
{
    memoryPool[SCA_STACK_SIZE_ADDRESS] -= size;
}