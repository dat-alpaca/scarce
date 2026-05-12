#include "ui/ui.h"
#include <scarce.h>

void on_load_view(engine* e, memory_pool* pool)
{
    e->log_error(e->logger, "hi", 2);
}

void on_update_view(engine* e, memory_pool* pool)
{
    ui_state* state = e->ui_begin_stack(pool, e->renderer);
    e->ui_clear(e);

    {
        u8* value = e->scarce_push(pool, 5);
        value[0] = 20;
        value[1] = 10;
        value[2] = 5;
        value[3] = 3;
        value[4] = 4;

        e->ui_hsml(state, "test.hsml");
        e->ui_end(state);

        e->scarce_pop(pool, 5);
    }
    
    return;

    //e->ui_set_position(state, POS_BOTTOM, 0);
    e->ui_set_align(state, UI_ALIGN_LEFT, 0);

    ui_button* button = (ui_button*)&pool[700];
    e->ui_button_render(button, state, "press");
    e->ui_button_update(button, state, e);

    // txt:
    e->ui_feed(state);
    ui_text_box* textbox = (ui_text_box*)&pool[760];
    e->ui_text_box_render(textbox, state);
    e->ui_text_box_update(textbox, state, e);

    

    // Misc:
    if(e->is_mouse_btn_pressed(e->window, SCA_MOUSE_LEFT))
    {
        e->log_warn(e->logger, "hello", 5);
    }

    e->renderer_set_character_background_color(e->renderer, 0, 0, 1.0f, 1.0f, 0.0f, true);
    e->renderer_set_character_background_color(e->renderer, 1, 1, 0.0f, 1.0f, 0.0f, true);
    e->renderer_set_character_background_color(e->renderer, 5, 5, 0.0f, 1.0f, 0.0f, true);

    u32 mouseX, mouseY;
    e->get_mouse_position(e->window, e->renderer, &mouseX, &mouseY);

    e->renderer_set_character_background_color(e->renderer, mouseX, mouseY, 0.0f, 0.0f, 0.0f, true);
}