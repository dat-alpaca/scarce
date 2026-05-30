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
    pool[127] = !pool[127];
}

void on_load_view(engine* e, memory_pool* pool)
{
    e->log_info(e->logger, "testbed on_load_view", 21);

    text_color color = { 0 };
    color.color = SY_COLOR_WHITE;
    color.colorFaint = true;

    text_color hovered = color;
    hovered.colorFaint = false;

    // language option
    pool[127] = 0; 

    // Quit Button:
    ui_button* quitBtn = (ui_button*)&pool[128 + 16];
    e->ui_button_init(quitBtn, quit_button_callback, &color, &hovered, 4);
    
    // Language:
    ui_button* langButton = (ui_button*)&pool[128 + 32];
    langButton->alignMiddle = true;
    e->ui_button_init(langButton, language_button_callback, &color, &hovered, 6);

    // Textbox:
    ui_text_box* textbox = (ui_text_box*)&pool[128 + 48];
    e->ui_text_box_init(textbox, &pool[128 + 48 + 16], &color, &hovered, 6);
}

void on_update_view(engine* e, memory_pool* pool)
{
    ui_state* state = (ui_state*)&pool[512];
    e->ui_begin(state, pool, e->renderer);

    ui_button* quitBtn = (ui_button*)&pool[128 + 16];
    ui_button* langButton = (ui_button*)&pool[128 + 32];
    ui_text_box* textbox = (ui_text_box*)&pool[128 + 48];

    e->ui_button_update(quitBtn, state, e);
    e->ui_button_update(langButton, state, e);
    e->ui_text_box_update(textbox, state, e);
}

void on_render_view(engine* e, memory_pool* pool)
{
    ui_state* state = (ui_state*)&pool[512];
    ui_text_box* textbox = (ui_text_box*)&pool[128 + 48];

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
        value[0] = 128 + 16;    // %0 - quit btn
        value[1] = 128 + 32;    // %1 - lang btn
        value[2] = 128 + 48;    // %2 - textbox
        value[3] = pool[127];   // %3 - language option
        value[4] = hovered;     // %4 - is textbox hovered

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

void on_unload_view(engine* e, memory_pool* pool)
{
    e->log_info(e->logger, "View unloaded", 14);
}