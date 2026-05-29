#pragma once
#include "memory/linear_arena.h"
#include "memory/stack_arena.h"
#include "memory/tag.h"

typedef struct
{
    linear_arena arena[SCA_LINEAR_TAGS];
    stack_arena transientArena;
} memory_system;

extern memory_system gMemorySystem;

struct
{
    u64 memoryAmountPerTag[TAG_AMOUNT];
} typedef memory_options;
void memory_system_init(memory_options* options);

void* sca_allocate(memory_tag tag, void* data, u32 size, u8 alignment);

struct
{
    u64 memoryCapacity;
    u64 memoryUsed;
} typedef memory_status;
memory_status memory_system_status(memory_tag tag);