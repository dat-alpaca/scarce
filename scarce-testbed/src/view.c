#include <scarce.h>

void on_load_view(engine* e, memory_pool* pool)
{
    e->log_error(e->logger, "hi", 2);
}

void on_update_view(engine* e, memory_pool* pool)
{
    // render:
    e->ui_clear(e);
    ui_state* state = e->ui_begin_stack(pool, e->renderer);
    
    text_color color = { 0 };
    color.color = SY_COLOR_WHITE;

    e->ui_set_color(state, &color);

    e->ui_sameline(state, true);
    e->ui_text(state, "hell", 4);
    e->ui_text(state, "hell2", 5);
    e->ui_sameline(state, false);
    e->ui_space(state, 1);
    e->ui_feed(state);

    //e->ui_set_position(state, POS_BOTTOM, 0);
    e->ui_set_align(state, ALIGN_LEFT, 0);

    ui_button* button = (ui_button*)&pool[700];
    e->ui_button_render(button, state, "press");
    e->ui_button_update(button, state, e);

    // txt:
    e->ui_feed(state);
    ui_text_box* textbox = (ui_text_box*)&pool[760];
    e->ui_text_box_render(textbox, state);
    e->ui_text_box_update(textbox, state, e);

    e->ui_end(state);

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