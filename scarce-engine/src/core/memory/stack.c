#include "stack.h"
#include "memory/memory.h"
#include <assert.h>

void* scarce_push(memory_pool* memoryPool, u16 size)
{
    assert(memoryPool);
    assert(size > 0);

    memoryPool[SCA_STACK_SIZE_ADDRESS] += size;
    return memoryPool + SCA_STACK_ADDRESS + memoryPool[SCA_STACK_SIZE_ADDRESS] - size;
}
void scarce_pop(memory_pool* memoryPool, u16 size)
{
    assert(memoryPool);
    assert(size > 0);

    memoryPool[SCA_STACK_SIZE_ADDRESS] -= size;
}