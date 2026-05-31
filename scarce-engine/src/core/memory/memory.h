#pragma once
#include "core/defines.h"

typedef u8 memory_pool;

// Stack:
#define SCA_STACK_SIZE_ADDRESS     (0x0000)
#define SCA_STACK_ADDRESS          (0x0001)

// Helpers:
bool is_power_of_two(u64 number);
u64 get_aligned_value(u64 address, u8 alignment);