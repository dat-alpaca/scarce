#include "memory_system.h"
#include "logging/logger.h"
#include "memory/linear_arena.h"
#include "memory/stack_arena.h"
#include "memory/tag.h"
#include "platform/platform.h"
#include <assert.h>

memory_system gMemorySystem;

void memory_system_init(memory_options* options)
{
    assert(options);

    linear_arena_init(&gMemorySystem.arena[TAG_UNKNOWN], options->memoryAmountPerTag[TAG_UNKNOWN]);
    linear_arena_init(&gMemorySystem.arena[TAG_SYSTEM], options->memoryAmountPerTag[TAG_SYSTEM]);
    linear_arena_init(&gMemorySystem.arena[TAG_USER], options->memoryAmountPerTag[TAG_USER]);

    gMemorySystem.generalCapacity[TAG_ASSETS - TAG_GENERAL_START] = options->memoryAmountPerTag[TAG_ASSETS];
    gMemorySystem.generalCapacity[TAG_RENDERER - TAG_GENERAL_START] = options->memoryAmountPerTag[TAG_RENDERER];
    gMemorySystem.generalCapacity[TAG_GENERAL - TAG_GENERAL_START] = options->memoryAmountPerTag[TAG_GENERAL];

    stack_arena_init(&gMemorySystem.transientArena, options->memoryAmountPerTag[TAG_TRANSIENT]);
}

void* sca_allocate(memory_tag tag, void* data, u32 size, u8 alignment)
{    
    if(is_linear_memory_tag(tag))
    {
        linear_arena* arena = &gMemorySystem.arena[tag];
        return linear_arena_push(arena, data, size, alignment);
    }

    if (tag == TAG_TRANSIENT)
    {
        stack_arena* arena = &gMemorySystem.transientArena;
        return stack_arena_push(arena, data, size, alignment);
    }

    // General:
    gMemorySystem.usedGeneralSize[tag - TAG_GENERAL_START] += size;
    return platform_allocate(size);
}
void sca_free(memory_tag tag, void* address, u32 size)
{
    if(is_linear_memory_tag(tag))
        log_critical_s("attempted to deallocate non-transient data", 43);

    if (tag == TAG_TRANSIENT)
        return stack_arena_pop(&gMemorySystem.transientArena, size);

    // General:
    gMemorySystem.usedGeneralSize[tag - TAG_GENERAL_START] -= size;
    platform_deallocate(address);
}

memory_status memory_system_status(memory_tag tag)
{
    assert(tag >= TAG_UNKNOWN && tag < TAG_AMOUNT);

    memory_status status = { 0 };

    if (is_linear_memory_tag(tag))
    {
        status.memoryCapacity = gMemorySystem.arena[tag].capacity;
        status.memoryUsed = gMemorySystem.arena[tag].current;
        return status;
    }

    if (tag == TAG_TRANSIENT)
    {
        status.memoryCapacity = gMemorySystem.transientArena.capacity;
        status.memoryUsed = gMemorySystem.transientArena.current;
        return status;
    }

    status.memoryCapacity = gMemorySystem.generalCapacity[tag - TAG_GENERAL_START];
    status.memoryUsed = gMemorySystem.usedGeneralSize[tag - TAG_GENERAL_START];
    return status;
}