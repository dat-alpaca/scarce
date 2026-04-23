#include "fixed_array.h"
#include <stdlib.h>

void fixed_array_init(fixed_array* array, u32 capacity)
{
    if (!array)
        return;

    array->capacity = capacity;
    array->current = 0;
    array->buffer = malloc(capacity);
    if (!array->buffer)
        return;
}

void fixed_array_push(fixed_array* array, void* data, u32 bytes)
{
    if (!array || !data || bytes <= 0)
        return;

    if (array->current + bytes > array->capacity)
        return;

    u8* dataPtr = (u8*)data;
    u8* buffer = (u8*)array->buffer;
    for (u32 i = 0; i < bytes; ++i)
        buffer[i] = dataPtr[i];
}

void fixed_array_pop(fixed_array* array, u32 bytes)
{
    array->current -= bytes;
}

void fixed_array_destroy(fixed_array* array)
{
    if (!array)
        return;

    free(array->buffer);
    array->capacity = 0;
    array->current = 0;
}