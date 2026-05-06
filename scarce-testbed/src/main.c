#include "platform/mouse.h"
#include <scarce.h>

static engine* _e = 0;

static void display_random_characters()
{
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

static void clear_buffer()
{
    for(u32 x = 0; x < _e->renderer_width(_e->renderer); ++x)
    {
        for(u32 y = 0; y < _e->renderer_height(_e->renderer); ++y)
        {
            _e->renderer_set_character_color(_e->renderer, x, y, 0.0f, 0.0f, 0.0f);
            _e->renderer_set_character_background_color(_e->renderer, x, y, 0.0f, 0.0f, 0.0f, false);
        }
    }
}

void on_load(memory_pool* pool, engine* engine)
{
    _e = engine;

    _e->logger_set_header(_e->logger, false);
    _e->renderer_set_character_size(_e->renderer, 24);
}

bool on_update(memory_pool* pool)
{
    clear_buffer();

    if(_e->is_mouse_btn_pressed(_e->window, SCA_MOUSE_LEFT))
    {
        _e->log_warn(_e->logger, "hello", 5);
    }

    _e->renderer_set_character_background_color(_e->renderer, 0, 0, 0.0f, 1.0f, 0.0f, true);
    _e->renderer_set_character_background_color(_e->renderer, 1, 1, 0.0f, 1.0f, 0.0f, true);
    _e->renderer_set_character_background_color(_e->renderer, 5, 5, 0.0f, 1.0f, 0.0f, true);

    u32 mouseX, mouseY;
    _e->get_mouse_position(_e->window, _e->renderer, &mouseX, &mouseY);

    _e->renderer_set_character_background_color(_e->renderer, mouseX, mouseY, 0.0f, 0.0f, 0.0f, true);

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