#pragma once
#include "core/defines.h"
#include "memory/memory.h"

#define SCA_VIEW_INVALID (-1)

struct engine;
typedef u16 view_id;

typedef void (*on_view_load)(struct engine* e, memory_pool* pool);
typedef void (*on_view_update)(struct engine* e, memory_pool* pool);
typedef void (*on_view_render)(struct engine* e, memory_pool* pool);

typedef struct
{
    view_id id;
    on_view_load load;
    on_view_update update;
    on_view_render render;
} view_data;

typedef struct
{
    view_data* data;
    u32 count;
    u32 capacity;
    u32 currentViewIndex;
} view_holder;

void view_holder_init(view_holder* holder, u32 capacity);
void view_holder_destroy(view_holder* holder);
void view_holder_register(view_holder* holder, view_data* data);

void view_holder_switch_view(view_holder* holder, struct engine* e, memory_pool* pool, view_id id);
view_data* view_holder_current(view_holder* holder);