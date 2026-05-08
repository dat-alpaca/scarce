#include <assert.h>

#include "scarce.h"
#include "view.h"
#include "logging/logger.h"
#include "platform/platform.h"

#define SCA_VIEW_MAX_CAPACITY 32

void view_holder_init(view_holder* holder, u32 capacity)
{
    assert(holder);
    assert(capacity > 0);
    assert(capacity < SCA_VIEW_MAX_CAPACITY);

    holder->currentViewIndex = SCA_VIEW_INVALID;
    holder->count = 0;
    holder->capacity = capacity;
    holder->data = platform_mmap(NULL, capacity * sizeof(view_data), PROTECTION_READ | PROTECTION_WRITE, MEMORY_ANON |MEMORY_PRIVATE, invalid_file_descriptor, 0);
}
void view_holder_destroy(view_holder* holder)
{
    assert(holder);
    platform_munmmap(holder->data, holder->capacity * sizeof(view_data));
    holder->data = NULL;
}

void view_holder_register(view_holder* holder, view_data* data)
{
    assert(data);
    assert(holder);
    assert(holder->data);
    assert(holder->count + 1 < SCA_VIEW_MAX_CAPACITY);

    holder->data[holder->count] = *data;
    ++holder->count;
}

void view_holder_switch_view(view_holder* holder, engine* e, memory_pool* pool, view_id id)
{
    assert(holder);
    assert(holder->data);
    assert(e);
    assert(pool);

    u32 index = SCA_VIEW_INVALID;
    for(u32 i = 0; i < holder->capacity; ++i)
    {
        if(holder->data[i].id == id)
            index = i;
    }

    if (index == -1)
        log_critical(e->logger, "Invalid view_id", 16);

    holder->currentViewIndex = index;
    holder->data[index].load(e, pool);
}

view_data* view_holder_current(view_holder* holder)
{
    assert(holder);
    assert(holder->data);
    assert(holder->currentViewIndex >= 0);
    assert(holder->currentViewIndex < SCA_VIEW_MAX_CAPACITY);

    return &holder->data[holder->currentViewIndex];
}