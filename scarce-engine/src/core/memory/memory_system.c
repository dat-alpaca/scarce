#include "memory_system.h"
#include "memory/linear_arena.h"
#include "memory/stack_arena.h"
#include "memory/tag.h"
#include <assert.h>

memory_system gMemorySystem;

void memory_system_init(memory_options* options)
{
    assert(options);

    for (u8 tag = TAG_UNKNOWN; tag < SCA_LINEAR_TAGS; ++tag)
    {
        u64 memoryRequested = options->memoryAmountPerTag[tag];
        linear_arena_init(&gMemorySystem.arena[tag], memoryRequested);
    }

    stack_arena_init(&gMemorySystem.transientArena, options->memoryAmountPerTag[TAG_TRANSIENT]);
}

void* sca_allocate(memory_tag tag, void* data, u32 size, u8 alignment)
{    
    if (tag == TAG_TRANSIENT)
    {
        stack_arena* arena = &gMemorySystem.transientArena;
        return stack_arena_push(arena, data, size, alignment);
    }

    linear_arena* arena = &gMemorySystem.arena[tag];
    return linear_arena_push(arena, data, size, alignment);
}

memory_status memory_system_status(memory_tag tag)
{
    assert(tag >= TAG_UNKNOWN && tag < TAG_AMOUNT);

    memory_status status = { 0 };

    if (tag == TAG_TRANSIENT)
    {
        status.memoryCapacity = gMemorySystem.transientArena.capacity;
        status.memoryUsed = gMemorySystem.transientArena.current;
        return status;
    }

    status.memoryCapacity = gMemorySystem.arena[tag].capacity;
    status.memoryUsed = gMemorySystem.arena[tag].current;

    return status;
}