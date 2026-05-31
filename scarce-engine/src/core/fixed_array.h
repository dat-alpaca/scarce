#pragma once
#include "defines.h"
#include "memory/tag.h"

typedef struct fixed_array
{
    void* buffer;
    u32 capacity;
    u32 current;
    u32 elementSize;
    memory_tag tag;
} fixed_array;

void  fixed_array_init(fixed_array* array, u32 elementCount, u32 elementSize, memory_tag tag);
void* fixed_array_get(fixed_array* array, u32 index);
u32   fixed_array_size(fixed_array* array);
bool  fixed_array_empty(fixed_array* array);

void* fixed_array_push(fixed_array* array, void* data, u32 count);
void  fixed_array_pop(fixed_array* array, u32 count);

void  fixed_array_clear(fixed_array* array);
void  fixed_array_destroy(fixed_array* array);