#include "memory.h"
#include <assert.h>

bool is_power_of_two(u64 number)
{
    return (number > 0) && ((number & (number - 1)) == 0);
}

u64 get_aligned_value(u64 address, u8 alignment)
{
    assert(is_power_of_two(alignment));
    return (address + alignment - 1) & ~(alignment - 1);
}