#pragma once
#include "defines.h"

typedef struct linear_arena
{
    void* data;

    u32 current;
    u32 capacity;
} linear_arena;

void  linear_arena_init(linear_arena* arena, u32 capacity);
void* linear_arena_push(linear_arena* arena, void* data, u32 size, u8 alignment);
void  linear_arena_destroy(linear_arena* arena);