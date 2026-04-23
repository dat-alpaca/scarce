#pragma once
#include "core/defines.h"
#include "scarce.h"

#define _ONE_SECOND  (1000000000)
#define _HALF_SECOND (1000000000 / 2)
#define _ONE_FRAME   (33333333)
#define _HALF_FRAME  (33333333 / 2)

const u8 get_stack_remaining_size(memory_pool* pool);

u8 open_file(engine* e, memory_pool* pool, const char* filename, file_mode mode);