#include "scrollbar.h"
#include <assert.h>
#include "scarce.h"
#include "ui/ui.h"

void ui_scrollbar_init(ui_scrollbar* scrollbar, container* container, text_color* color, text_color* hoveredColor, char symbol)
{
    assert(scrollbar);
    assert(color);
    assert(hoveredColor);
    assert(container);
    assert(symbol >= SCA_FONT_START_INDEX && symbol <= SCA_FONT_END_INDEX);

    scrollbar->container = container;
    scrollbar->color = *color;
    scrollbar->hoveredColor = *hoveredColor;

    scrollbar->symbol = symbol;

    scrollbar->yOffset = 0;
    scrollbar->isRendered = 0;
}

void ui_scrollbar_render(ui_scrollbar* scrollbar, ui_state* state)
{
    assert(scrollbar);
    assert(state);

    container* container = scrollbar->container;

    ui_switch_container(state, container);
    scrollbar->isRendered = true;
    
    if (!container->overflow && (container->overflow || !scrollbar->alwaysShow))
        return;

    ui_vline(state, container->width - 1, scrollbar->symbol);
}

void ui_scrollbar_update(ui_scrollbar* scrollbar, ui_state* state, struct engine* e)
{
    if (!scrollbar->isRendered)
        return;
}