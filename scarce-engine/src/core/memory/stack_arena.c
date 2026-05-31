#include "stack_arena.h"
#include "memory/memory.h"
#include "platform/platform.h"

#include <assert.h>
#include <stdalign.h>
#include <string.h>

void stack_arena_init(stack_arena* arena, u32 capacity)
{
    assert(arena);
    assert(capacity > 0);

    arena->data = platform_allocate(capacity);
    assert(arena->data);

    arena->capacity = capacity;
    arena->current = 0;
}

void* stack_arena_push(stack_arena* arena, void* data, u32 size, u8 alignment)
{
    assert(arena);

    u64 alignedOffset = get_aligned_value(arena->current, alignment);
    assert(alignedOffset + size <= arena->capacity);

    void* alignedData = arena->data + alignedOffset;
    if (data && size > 0)
        memcpy(alignedData, data, size);

    arena->current = alignedOffset + size;
    return alignedData;
}

void stack_arena_pop(stack_arena* arena, u32 size)
{
    assert(arena);
    arena->current -= size;
}
void stack_arena_clear(stack_arena* arena)
{
    assert(arena);
    arena->current = 0;
}

void stack_arena_destroy(stack_arena* arena)
{
    assert(arena);
    assert(arena->data);

    platform_deallocate(arena->data);
    arena->data = NULL;
    arena->capacity = 0;
    arena->current = 0;
}