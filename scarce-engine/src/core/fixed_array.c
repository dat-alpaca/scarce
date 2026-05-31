#include "fixed_array.h"

#include "defines.h"
#include "memory/tag.h"
#include "memory/memory_system.h"

#include <assert.h>
#include <string.h>

void fixed_array_init(fixed_array* array, u32 elementCount, u32 elementSize, memory_tag tag)
{
    assert(array);

    array->tag = tag;
    array->elementSize = elementSize;
    array->capacity = elementSize * elementCount;
    array->current = 0;

    array->buffer = sca_allocate(tag, NULL, array->capacity, 1);
    assert(array->buffer);

    memset(array->buffer, 0, array->capacity);
}

void* fixed_array_get(fixed_array* array, u32 index)
{
    u32 offset = index * array->elementSize;
    assert(array && array->buffer);
    assert(offset >= 0);
    assert(offset + array->elementSize <= array->current);

    return (u8*)array->buffer + offset;
}
u32 fixed_array_size(fixed_array* array)
{
    assert(array);
    return array->current / array->elementSize;
}
bool fixed_array_empty(fixed_array* array)
{
    return fixed_array_size(array) == 0;
}

void* fixed_array_push(fixed_array* array, void* data, u32 count)
{
    assert(array);

    u32 bytes = count * array->elementSize;
    assert(array->current + bytes <= array->capacity);
    
    if (data)
        memcpy((u8*)array->buffer + array->current, data, bytes);
    else
        memset(array->buffer + array->current, 0, bytes);

    void* previous = array->buffer + array->current;
    array->current += bytes;

    return previous;
}

void fixed_array_pop(fixed_array* array, u32 count)
{
    u32 bytes = count * array->elementSize;
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