#include <scarce.h>

static engine* _e = 0;

void on_load(memory_pool* pool, engine* engine)
{
    _e = engine;
}

bool on_update(memory_pool* pool)
{
    return true;
}

void on_unload(memory_pool* pool)
{
    /* Blank */
}  

SECTION_ENTRY const exports _ = {
    .on_load   = on_load,
    .on_update = on_update,
    .on_unload = on_unload
};