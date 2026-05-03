#pragma once
#include "defines.h"

typedef struct
{
    void* buffer;
    u32 capacity;
    u32 current;
} fixed_array;

void fixed_array_init(fixed_array* array, u32 capacity);
void* fixed_array_get(fixed_array* array, u32 index, u32 elementSize);

void fixed_array_push(fixed_array* array, void* data, u32 bytes);
void fixed_array_pop(fixed_array* array, u32 bytes);


void fixed_array_clear(fixed_array* array);
void fixed_array_destroy(fixed_array* array);