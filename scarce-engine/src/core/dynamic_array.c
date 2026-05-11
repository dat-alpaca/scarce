#include "dynamic_array.h"
#include "logging/logger.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define SCA_DA_GROWTH_RATIO 1.618

void dynamic_array_init(dynamic_array* array, u32 capacity)
{
    assert(array);

    array->capacity = capacity;
    array->current = 0;

    array->buffer = malloc(capacity);
    assert(array->buffer);
    memset(array->buffer, 0, capacity);
}

void* dynamic_array_get(dynamic_array* array, u32 index, u32 elementSize)
{
    u32 offset = index * elementSize;
    assert(array && array->buffer);
    assert(offset >= 0);
    assert(offset + elementSize <= array->current);

    return (u8*)array->buffer + offset;
}

void dynamic_array_resize(dynamic_array* array, u32 newSize)
{
    assert(array);
    assert(newSize > 0);

    if (newSize == array->capacity)
        return;

    u32 usedSize = (newSize < array->current) ? newSize : array->current;
    if (newSize < array->current)
    {
        log_warn_s("Resized dynamic array down. Data will be lost.", 47);
        array->current = newSize;
    }

    u8* tempBuffer = (u8*)malloc(newSize);
    assert(tempBuffer);

    if (array->buffer)
    {
        memcpy(tempBuffer, array->buffer, usedSize);
        free(array->buffer);
    }
    
    array->buffer = tempBuffer;
    array->capacity = usedSize;
}

void dynamic_array_push(dynamic_array* array, void* data, u32 bytes)
{
    assert(array && data && bytes > 0);

    if (array->current + bytes > array->capacity)
        dynamic_array_resize(array, (array->capacity + 1) * SCA_DA_GROWTH_RATIO);

    memcpy((u8*)array->buffer + array->current, data, bytes);
    array->current += bytes;
}

void dynamic_array_pop(dynamic_array* array, u32 bytes)
{
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