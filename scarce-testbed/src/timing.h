#pragma once
#include "core/memory/memory.h"
#include "scarce.h"

void calculate_delta_time(engine* e, memory_pool* pool);
void increment_accumulator_time(engine* e, memory_pool* pool, u32 accumulatorAddress);