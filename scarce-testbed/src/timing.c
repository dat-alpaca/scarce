#include "timing.h"
#include "map.h"

void calculate_delta_time(engine* e, memory_pool* pool)
{
    u64* currentTime = e->scarce_push(pool, sizeof(u64));
    *currentTime = e->get_timestamp_ns();

    u32* lastTime = e->scarce_read(pool, SCA_US_LAST_TIME_ADDR);
    u32* delta = e->scarce_read(pool, SCA_US_DELTA_TIME_ADDR);
    
    *delta = (u32)(*currentTime - *lastTime);
    *lastTime = *currentTime;

    e->scarce_pop(pool, sizeof(u64));
}

void increment_accumulator_time(engine* e, memory_pool* pool, u32 accumulatorAddress)
{
    u32* delta = e->scarce_read(pool, SCA_US_DELTA_TIME_ADDR);
    u32* accumulator = e->scarce_read(pool, accumulatorAddress);

    *accumulator += *delta;
}