#include "defines.h"
#include "memory/memory.h"
#include "platform/mouse.h"
#include "ui/button.h"
#include "ui/text_box.h"
#include "ui/ui.h"
#include <scarce.h>

#include "view.h"
#include "view/view.h"

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
    e->requestExit = true;
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

    // textbox:
    ui_text_box* textbox = (ui_text_box*)&pool[760];
    char* textboxContents = (char*)&pool[900];
    _e->ui_text_box_init(textbox, textboxContents, &color, &hovered,  10);

    // view test:
    view_data* mainMenu = _e->scarce_push(pool, sizeof(view_data)); 
    {
        mainMenu->id = 0;
        mainMenu->load = on_load_view;
        mainMenu->render = NULL;
        mainMenu->update = on_update_view;
        
        _e->view_holder_register(_e->viewHolder, mainMenu);
        _e->scarce_pop(pool, sizeof(view_data)); 
    }

    _e->view_holder_switch_view(_e->viewHolder, _e, pool, 0);
}

bool on_update(memory_pool* pool)
{
    _e->view_holder_update(_e->viewHolder, _e, pool);
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