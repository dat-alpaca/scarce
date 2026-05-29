#include "dynamic_array.h"
#include "logging/logger.h"
#include "platform/platform.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define SCA_DA_GROWTH_RATIO 1.618

void dynamic_array_init(dynamic_array* array, u32 elementCount, u32 elementSize)
{
    assert(array);

    array->elementSize = elementSize;
    array->capacity = elementSize * elementCount;
    array->current = 0;

    array->buffer = platform_allocate(array->capacity);
    assert(array->buffer);
    memset(array->buffer, 0, array->capacity);
}

void* dynamic_array_get(dynamic_array* array, u32 index)
{
    u32 offset = index * array->elementSize;
    assert(array && array->buffer);
    assert(offset >= 0);
    assert(offset + array->elementSize <= array->current);

    return (u8*)array->buffer + offset;
}

void dynamic_array_resize(dynamic_array* array, u32 newElementCount)
{
    assert(array);
    assert(newElementCount > 0);

    u32 newSize = newElementCount * array->elementSize;
    if (newSize == array->capacity)
        return;

    u32 usedSize = (newSize < array->current) ? newSize : array->current;
    if (newSize < array->current)
    {
        log_warn_s("Resized dynamic array down. Data will be lost.", 47);
        array->current = newSize;
    }

    u8* tempBuffer = (u8*)platform_allocate(newSize);
    assert(tempBuffer);

    if (array->buffer)
    {
        memcpy(tempBuffer, array->buffer, usedSize);
        free(array->buffer);
    }
    
    array->buffer = tempBuffer;
    array->capacity = usedSize;
}
u32 dynamic_array_size(dynamic_array* array)
{
    return array->current / array->elementSize;
}
bool dynamic_array_empty(dynamic_array* array)
{
    return dynamic_array_size(array) == 0;
}

void* dynamic_array_push(dynamic_array* array, void* data, u32 count)
{
    assert(array && count > 0);

    u32 bytes = count * array->elementSize;
    if (array->current + bytes > array->capacity)
        dynamic_array_resize(array, (array->capacity + 1) * SCA_DA_GROWTH_RATIO);

    if (data)
        memcpy((u8*)array->buffer + array->current, data, bytes);

    void* previous = array->buffer + array->current;
    array->current += bytes;

    return previous;
}

void dynamic_array_pop(dynamic_array* array, u32 count)
{
    u32 bytes = count * array->elementSize;
    assert(array->current >= bytes);

    array->current -= bytes;
}

void dynamic_array_clear(dynamic_array* array)
{
    assert(array);
    array->current = 0;
}

void dynamic_array_destroy(dynamic_array* array)
{
    assert(array);
    assert(array->buffer);

    free(array->buffer);
    array->buffer = NULL;

    array->capacity = 0;
    array->current = 0;
}