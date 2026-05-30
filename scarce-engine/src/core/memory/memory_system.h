#pragma once
#include "memory/linear_arena.h"
#include "memory/stack_arena.h"
#include "memory/tag.h"

typedef struct
{
    linear_arena arena[TAG_LINEAR_END - TAG_LINEAR_START];
    stack_arena transientArena;

    u64 generalCapacity[TAG_GENERAL_END - TAG_LINEAR_START];
    u64 usedGeneralSize[TAG_GENERAL_END - TAG_LINEAR_START];
} memory_system;
extern memory_system gMemorySystem;

struct
{
    u64 memoryAmountPerTag[TAG_AMOUNT];
} typedef memory_options;
void memory_system_init(memory_options* options);

void* sca_allocate(memory_tag tag, void* data, u32 size, u8 alignment);
void sca_free(memory_tag tag, void* address, u32 size);

struct
{
    u64 memoryCapacity;
    u64 memoryUsed;
} typedef memory_status;
memory_status memory_system_status(memory_tag tag);