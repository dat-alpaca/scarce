#include "memory_system.h"
#include "defines.h"
#include "logging/logger.h"
#include "memory/linear_arena.h"
#include "memory/stack_arena.h"
#include "memory/tag.h"
#include "platform/platform.h"
#include <assert.h>

memory_system gMemorySystem;

static memory_tag get_general_memory_index(memory_tag tag)
{
    return tag - TAG_GENERAL_START;
}

void memory_system_init(memory_options* options)
{
    assert(options);

    linear_arena_init(&gMemorySystem.arena[TAG_UNKNOWN], options->memoryAmountPerTag[TAG_UNKNOWN]);
    linear_arena_init(&gMemorySystem.arena[TAG_SYSTEM], options->memoryAmountPerTag[TAG_SYSTEM]);
    linear_arena_init(&gMemorySystem.arena[TAG_USER], options->memoryAmountPerTag[TAG_USER]);

    for (memory_tag tag = TAG_GENERAL_START; tag < TAG_GENERAL_START + TAG_GENERAL_AMOUNT; ++tag)
    {
        u32 arrayIndex = get_general_memory_index(tag);
        gMemorySystem.generalStats[arrayIndex].capacity = options->memoryAmountPerTag[tag];
        gMemorySystem.generalStats[arrayIndex].used = 0;
    }

    stack_arena_init(&gMemorySystem.transientArena, options->memoryAmountPerTag[TAG_TRANSIENT]);
}

void* sca_allocate(memory_tag tag, void* data, u32 size, u8 alignment)
{    
    if(is_linear_system_memory_tag(tag))
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
    gMemorySystem.generalStats[get_general_memory_index(tag)].used += size;
    return platform_allocate(size);
}
void* sca_reallocate(memory_tag tag, void* data, u32 oldSize, u32 size)
{
    assert(data);
    assert(!is_linear_system_memory_tag(tag));

    if (tag == TAG_TRANSIENT)
    {
        stack_arena* arena = &gMemorySystem.transientArena;
        u32 previousSize = arena->current;
        stack_arena_clear(arena);
        
        void* base = stack_arena_push(arena, data, previousSize, 1);
        arena->current = size;
        return base;
    }

    // General:
    gMemorySystem.generalStats[get_general_memory_index(tag)].used -= oldSize;
    gMemorySystem.generalStats[get_general_memory_index(tag)].used += size;
    return platform_reallocate(data, size);
}
void sca_free(memory_tag tag, void* address, u32 size)
{
    if(is_linear_system_memory_tag(tag))
        log_critical_s("attempted to deallocate non-transient data", 43);

    if (tag == TAG_TRANSIENT)
        return stack_arena_pop(&gMemorySystem.transientArena, size);

    // General:
    gMemorySystem.generalStats[get_general_memory_index(tag)].used -= size;
    platform_deallocate(address);
}

memory_status memory_system_status(memory_tag tag)
{
    assert(tag >= TAG_START && tag < TAG_AMOUNT);

    memory_status status = { 0 };
    if (is_linear_system_memory_tag(tag))
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

    status.memoryCapacity = gMemorySystem.generalStats[get_general_memory_index(tag)].capacity;
    status.memoryUsed = gMemorySystem.generalStats[get_general_memory_index(tag)].used;
    return status;
}