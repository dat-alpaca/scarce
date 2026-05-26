#include "defines.h"
#include "memory/memory.h"
#include "ui/button.h"
#include <scarce.h>

#include "view.h"
#include "view/view.h"

static engine* _e = 0;

void button_callback(engine* e, memory_pool* pool, struct ui_button* button)
{
    e->log_warn(_e->logger, "pressed the button", 19);
    e->requestExit = true;
}

void on_load(memory_pool* pool, engine* engine)
{
    _e = engine;
    _e->logger_set_header(_e->logger, false);

    // stack:
    pool[SCA_STACK_SIZE_ADDRESS] = 0;

    // view test:
    view_data* mainMenu = _e->scarce_push(pool, sizeof(view_data)); 
    {
        mainMenu->id = 0;
        mainMenu->load = on_load_view;
        mainMenu->render = on_render_view;
        mainMenu->update = on_update_view;
        mainMenu->unload = on_unload_view;
        
        _e->view_holder_register(_e->viewHolder, mainMenu);
        _e->scarce_pop(pool, sizeof(view_data)); 
    }

    _e->view_holder_switch_view(_e->viewHolder, _e, pool, 0);
}

bool on_update(memory_pool* pool)
{
    _e->view_holder_update(_e->viewHolder, _e, pool);
    _e->view_holder_render(_e->viewHolder, _e, pool);
    return true;
}

SECTION_ENTRY const exports _ = {
    .on_load   = on_load,
    .on_update = on_update,
};