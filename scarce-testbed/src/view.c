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

static void language_button_callback(engine* e, memory_pool* pool, ui_button* button)
{
    u8 value = pool[300];
    if (value == 0)
        pool[300] = 1;
    if (value == 1)
        pool[300] = 0;
}

void on_load_view(engine* e, memory_pool* pool)
{
    e->log_info(e->logger, "testbed on_load_view", 21);

    text_color color = { 0 };
    color.color = SY_COLOR_WHITE;
    color.colorFaint = true;

    text_color hovered = color;
    hovered.colorFaint = false;

    // Quit Button:
    ui_button* button = (ui_button*)&pool[200];
    e->ui_button_init(button, quit_button_callback, &color, &hovered, 4);
    
    // Language:
    ui_button* langButton = (ui_button*)&pool[132];
    langButton->alignMiddle = true;
    e->ui_button_init(langButton, language_button_callback, &color, &hovered, 6);
    pool[300] = 0;

    // Textbox:
    ui_text_box* textbox = (ui_text_box*)&pool[100];
    e->ui_text_box_init(textbox, &pool[116], &color, &hovered, 6);
}

void on_update_view(engine* e, memory_pool* pool)
{
    ui_state* state = (ui_state*)&pool[1000];
    e->ui_begin(state, pool, e->renderer);

    ui_text_box* textbox = (ui_text_box*)&pool[100];
    ui_button* button = (ui_button*)&pool[200];
    ui_button* langButton = (ui_button*)&pool[132];

    e->ui_text_box_update(textbox, state, e);
    e->ui_button_update(button, state, e);
    e->ui_button_update(langButton, state, e);
}

void on_render_view(engine* e, memory_pool* pool)
{
    ui_state* state = (ui_state*)&pool[1000];
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
        u8* value = e->scarce_push(pool, sizeof(u8) * 5);
        value[0] = 100;         // %0
        value[1] = 200;         // %1
        value[2] = 132;         // %2
        value[3] = pool[300];   // %3
        value[4] = hovered;     // %4

        {
            // string
            u8* charCount = e->scarce_push(pool, sizeof(u8));
            *charCount = 2; // %5

            char* string = e->scarce_push(pool, sizeof(char) * *charCount);
            string[0] = 'e'; // %^6
            string[1] = 'n';
        }
        
        u8* count = e->scarce_push(pool, sizeof(u8));
        *count = 8 * sizeof(u8);

        e->ui_hsml(state, "assets/view.hsml");

        e->scarce_pop(pool, 8 * sizeof(u8) + 1);
    }
    
    e->ui_end(state);
}