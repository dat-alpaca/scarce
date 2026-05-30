#include "fixed_array.h"

#include "defines.h"
#include "memory/tag.h"
#include "memory/memory_system.h"

#include <assert.h>
#include <string.h>

void fixed_array_init(fixed_array* array, u32 capacity, memory_tag tag)
{
    assert(array);

    array->tag = tag;
    array->capacity = capacity;
    array->current = 0;

    array->buffer = sca_allocate(tag, NULL, capacity, 1);
    assert(array->buffer);

    memset(array->buffer, 0, capacity);
}

void* fixed_array_get(fixed_array* array, u32 index, u32 elementSize)
{
    u32 offset = index * elementSize;
    assert(array && array->buffer);
    assert(offset >= 0);
    assert(offset + elementSize <= array->current);

    return (u8*)array->buffer + offset;
}

void fixed_array_push(fixed_array* array, void* data, u32 bytes)
{
    assert(array);
    assert(array->current + bytes <= array->capacity);

    if (data)
        memcpy((u8*)array->buffer + array->current, data, bytes);
    
    array->current += bytes;
}

void fixed_array_pop(fixed_array* array, u32 bytes)
{
    assert(array->current >= bytes);
    array->current -= bytes;
}

void fixed_array_clear(fixed_array* array)
{
    assert(array);
    array->current = 0;
}

void fixed_array_destroy(fixed_array* array)
{
    assert(array);
    assert(array->buffer);

    sca_free(array->tag, array->buffer, array->capacity);
    array->buffer = NULL;

    array->capacity = 0;
    array->current = 0;
}