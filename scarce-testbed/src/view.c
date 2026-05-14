#include "memory/memory.h"
#include "physics/aabb.h"
#include "ui/button.h"
#include "ui/text_box.h"
#include "ui/ui.h"
#include <scarce.h>

static void quit_button_callback(engine* e, memory_pool* pool, ui_button* button)
{
    e->requestExit = true;
}

void on_load_view(engine* e, memory_pool* pool)
{
    e->log_info(e->logger, "testbed on_load_view", 21);

    text_color color = { 0 };
    color.color = SY_COLOR_WHITE;
    color.colorFaint = true;

    text_color hovered = color;
    hovered.colorFaint = false;

    // Button:
    ui_button* button = (ui_button*)&pool[200];
    e->ui_button_init(button, quit_button_callback, &color, &hovered, 4);

    // Textbox:
    ui_text_box* textbox = (ui_text_box*)&pool[100];
    e->ui_text_box_init(textbox, &pool[116], &color, &hovered, 6);
}

static void on_update(ui_state* state, engine* e)
{
    memory_pool* pool = state->pool;

    ui_text_box* textbox = (ui_text_box*)&pool[100];
    ui_button* button = (ui_button*)&pool[200];

    e->ui_text_box_update(textbox, state, e);
    e->ui_button_update(button, state, e);
}

static void on_render(ui_state* state, engine* e)
{
    memory_pool* pool = state->pool;
    ui_text_box* textbox = (ui_text_box*)&pool[100];

    e->ui_clear(e);

    // Mouse collision:
    bool hovered = false;
    {
        aabb* textboxAABB = (aabb*)e->scarce_push(pool, sizeof(aabb));
        aabb* mouseAABB = (aabb*)e->scarce_push(pool, sizeof(aabb));
        *textboxAABB = e->ui_text_box_aabb(textbox);
        *mouseAABB = e->ui_mouse_aabb(e);

        hovered = e->aabb_check_collision(*textboxAABB, *mouseAABB);
        e->scarce_pop(pool, sizeof(aabb));
        e->scarce_pop(pool, sizeof(aabb));
    }
    
    // Main HSML:
    {
        const u8 stackCount = 3;
        u8* value = e->scarce_push(pool, sizeof(u8) * stackCount);
        value[0] = 100;
        value[1] = 200;
        value[2] = stackCount - 1;

        e->ui_hsml(state, "assets/view.hsml");
        e->ui_end(state);

        e->scarce_pop(pool, sizeof(u8) * stackCount);
    }

    if (hovered)
    {
        e->ui_space(state, 2);
        e->ui_feed(state);

        state->color.colorFaint = true;
        e->ui_set_color(state, &state->color);
        e->ui_text(state, "Press the textbox & type", 25);
        e->ui_feed(state);

        e->ui_text(state, "BACKSPACE to erase", 19);
        e->ui_feed(state);
    }
}

void on_update_view(engine* e, memory_pool* pool)
{
    ui_state* state = e->ui_begin_stack(pool, e->renderer);
    on_update(state, e);
    on_render(state, e);
}