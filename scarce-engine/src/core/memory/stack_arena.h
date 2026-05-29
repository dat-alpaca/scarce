#pragma once
#include "defines.h"

typedef struct stack_arena
{
    void* data;

    u32 current;
    u32 capacity;
} stack_arena;

void  stack_arena_init(stack_arena* arena, u32 capacity);
void* stack_arena_push(stack_arena* arena, void* data, u32 size, u8 alignment);
void  stack_arena_pop(stack_arena* arena, u32 size);
void  stack_arena_destroy(stack_arena* arena);