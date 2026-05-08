#include "memory/memory.h"
#include "platform/mouse.h"
#include "ui/button.h"
#include "ui/ui.h"
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

void button_callback(engine* e, memory_pool* pool, struct ui_button* button)
{
    e->log_warn(_e->logger, "pressed the button", 19);
}

void on_load(memory_pool* pool, engine* engine)
{
    _e = engine;
    _e->logger_set_header(_e->logger, false);
    _e->renderer_set_character_size(_e->renderer, 24);

    // stack:
    pool[SCA_STACK_SIZE_ADDRESS] = 0;

    ui_button* button = (ui_button*)&pool[700];
    text_color color = { 0 };
    color.color = SY_COLOR_WHITE;
    color.colorFaint = true;

    text_color hovered = { 0 };
    hovered.colorFaint = false;
    hovered.color = SY_COLOR_BLUE;

    _e->ui_button_init(button, button_callback, &color, &hovered, 5);
}

bool on_update(memory_pool* pool)
{
    // render:
    _e->ui_clear(_e);
    ui_state* state = _e->ui_begin(pool, _e->renderer);
    
    text_color color = { 0 };
    color.color = SY_COLOR_WHITE;

    _e->ui_set_color(state, &color);
    _e->ui_text(state, "hello", 5);

    ui_button* button = (ui_button*)&pool[700];
    _e->ui_button_update(button, state, _e);
    _e->ui_button_render(button, state, "press");

    _e->ui_number(state, 10);

    _e->ui_end(state);

    // Misc:
    if(_e->is_mouse_btn_pressed(_e->window, SCA_MOUSE_LEFT))
    {
        _e->log_warn(_e->logger, "hello", 5);
    }

    _e->renderer_set_character_background_color(_e->renderer, 0, 0, 1.0f, 1.0f, 0.0f, true);
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