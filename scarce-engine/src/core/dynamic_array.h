#pragma once
#include "defines.h"

typedef struct
{
    void* buffer;
    u32 capacity;
    u32 current;
} dynamic_array;

void dynamic_array_init(dynamic_array* array, u32 capacity);
void* dynamic_array_get(dynamic_array* array, u32 index, u32 elementSize);

void dynamic_array_resize(dynamic_array* array, u32 newSize);

void dynamic_array_push(dynamic_array* array, void* data, u32 bytes);
void dynamic_array_pop(dynamic_array* array, u32 bytes);

void dynamic_array_clear(dynamic_array* array);
void dynamic_array_destroy(dynamic_array* array);