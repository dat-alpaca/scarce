#pragma once
#include "defines.h"

typedef struct
{
    void* buffer;
    u32 capacity;
    u32 current;
    u32 elementSize;
} dynamic_array;

void dynamic_array_init(dynamic_array* array, u32 elementCount, u32 elementSize);
void* dynamic_array_get(dynamic_array* array, u32 index);

void dynamic_array_resize(dynamic_array* array, u32 newElementCount);
u32 dynamic_array_size(dynamic_array* array);
bool dynamic_array_empty(dynamic_array* array);

void dynamic_array_push(dynamic_array* array, void* data, u32 count);
void dynamic_array_pop(dynamic_array* array, u32 count);

void dynamic_array_clear(dynamic_array* array);
void dynamic_array_destroy(dynamic_array* array);