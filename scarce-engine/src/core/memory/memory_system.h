#pragma once
#include "memory/linear_arena.h"
#include "memory/stack_arena.h"
#include "memory/tag.h"

#define SCA_MEMORY_SYSTEM_MAX_TAGS 32

typedef struct
{
    u64 capacity;
    u64 used;
} memory_system_general_stats;

typedef struct
{
    linear_arena arena[TAG_LINEAR_AMOUNT];
    stack_arena transientArena;
    memory_system_general_stats generalStats[SCA_MEMORY_SYSTEM_MAX_TAGS];
} memory_system;
extern memory_system gMemorySystem;

struct
{
    u64 memoryAmountPerTag[TAG_AMOUNT];
} typedef memory_options;
void memory_system_init(memory_options* options);

void* sca_allocate(memory_tag tag, void* data, u32 size, u8 alignment);
void* sca_reallocate(memory_tag tag, void* data, u32 oldSize, u32 size);
void sca_free(memory_tag tag, void* address, u32 size);

struct
{
    u64 memoryCapacity;
    u64 memoryUsed;
} typedef memory_status;
memory_status memory_system_status(memory_tag tag);