#pragma once
#include "core/defines.h"

struct
{
    u64 size;
} typedef general_allocator;

typedef u32 general_allocator_tag;

void  general_allocator_init(general_allocator* allocator);
void  general_allocator_register_tag(general_allocator* allocator, general_allocator_tag tag);
void* general_allocator_allocate(general_allocator* allocator, u32 size);
void  general_allocator_free(general_allocator* allocator, void* address);