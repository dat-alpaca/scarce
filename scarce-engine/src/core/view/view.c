#include <assert.h>
#include <stdint.h>

#include "scarce.h"
#include "view.h"
#include "logging/logger.h"

#define SCA_VIEW_MAX_CAPACITY 32

void view_holder_init(view_holder* holder, u32 capacity)
{
    assert(holder);
    assert(capacity > 0);
    assert(capacity <= SCA_VIEW_MAX_CAPACITY);

    holder->currentViewIndex = SCA_VIEW_INVALID;
    holder->count = 0;
    holder->capacity = capacity;
    holder->data = malloc(capacity * sizeof(view_data));
}
void view_holder_destroy(view_holder* holder)
{
    assert(holder);

    free(holder->data);
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
    for(u32 i = 0; i < holder->count; ++i)
    {
        if(holder->data[i].id != id)
            continue;

        index = i;
        break;
    }

    if (index == SCA_VIEW_INVALID)
        log_critical(e->logger, "Invalid view_id", 16);

    if (holder->currentViewIndex != SCA_VIEW_INVALID && holder->data[holder->currentViewIndex].unload)
    {
        on_view_unload unloadFunction = (on_view_unload)((uintptr_t)holder->data[holder->currentViewIndex].unload + (uintptr_t)e->baseAddress);    
        unloadFunction(e, pool);
    }

    holder->currentViewIndex = index;
    if (!holder->data[index].load)
        return;

    on_view_load loadFunction = (on_view_load)((uintptr_t)holder->data[index].load + (uintptr_t)e->baseAddress);    
    loadFunction(e, pool);
}

void view_holder_update(view_holder* holder, struct engine* e, memory_pool* pool)
{
    assert(holder);
    assert(pool);
    assert(e);

    view_data* data = view_holder_current(holder);
    if (!data->update)
        return;

    on_view_update updateFunction = (on_view_update)((uintptr_t)data->update + (uintptr_t)e->baseAddress);    
    if (updateFunction)
        updateFunction(e, pool);
}
void view_holder_render(view_holder* holder, struct engine* e, memory_pool* pool)
{
    assert(holder);
    assert(pool);
    assert(e);

    view_data* data = view_holder_current(holder);
    if (!data->render)
        return;

    on_view_render renderFunction = (on_view_render)((uintptr_t)data->render + (uintptr_t)e->baseAddress);    
    if (renderFunction)
        renderFunction(e, pool);
}

view_data* view_holder_current(view_holder* holder)
{
    assert(holder);
    assert(holder->data);
    assert(holder->currentViewIndex >= 0);
    assert(holder->currentViewIndex < SCA_VIEW_MAX_CAPACITY);

    return &holder->data[holder->currentViewIndex];
}