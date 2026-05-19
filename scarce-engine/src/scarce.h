#pragma once
#include "engine.h"

typedef void (*load_func)(memory_pool*, engine*);
typedef bool (*update_func)(memory_pool*);

typedef struct
{
    void (*on_load)(memory_pool*, engine*);
    bool (*on_update)(memory_pool*);
} exports;

