#include <scarce.h>

static engine* _e = 0;

void on_load(memory_pool* pool, engine* engine)
{
    _e = engine;

    for(u32 x = 0; x < _e->renderer_width(_e->renderer); ++x)
    {
        for(u32 y = 0; y < _e->renderer_height(_e->renderer); ++y)
        {
            _e->renderer_set_character_background_color(_e->renderer, x, y, 
                _e->random_float(0.0f, 1.0f), _e->random_float(0.0f, 1.0f), _e->random_float(0.0f, 1.0f), true
            );

            _e->renderer_set_character_color(_e->renderer, x, y, 
                _e->random_float(0.0f, 1.0f), _e->random_float(0.0f, 1.0f), _e->random_float(0.0f, 1.0f)
            );

            _e->renderer_set_character_letter(_e->renderer, x, y, _e->random_int('a', 'z'));
        }
    }
}

bool on_update(memory_pool* pool)
{
    static u64 a = 0;
    static bool b = false;
    a += 1;

    if (a > 100)
    {
        a = 0;
        if(b)
        {
            _e->renderer_set_character_size(_e->renderer, 16);
            b = !b;
        }
        else
        {
            _e->renderer_set_character_size(_e->renderer, 32);
            b = !b;
        }
    }

    return true;
    
    for(u32 x = 0; x < _e->renderer_width(_e->renderer); ++x)
    {
        for(u32 y = 0; y < _e->renderer_height(_e->renderer); ++y)
        {
            _e->renderer_set_character_background_color(_e->renderer, x, y, 
                _e->random_float(0.0f, 1.0f), _e->random_float(0.0f, 1.0f), _e->random_float(0.0f, 1.0f), true
            );

            _e->renderer_set_character_color(_e->renderer, x, y, 
                _e->random_float(0.0f, 1.0f), _e->random_float(0.0f, 1.0f), _e->random_float(0.0f, 1.0f)
            );

            _e->renderer_set_character_letter(_e->renderer, x, y, _e->random_int('a', 'z'));
        }
    }

    return true;
}

void on_unload(memory_pool* pool)
{
    
}  

SECTION_ENTRY const exports _ = {
    .on_load   = on_load,
    .on_update = on_update,
    .on_unload = on_unload
};