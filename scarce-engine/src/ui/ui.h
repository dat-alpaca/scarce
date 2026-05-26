#pragma once
#include "batch_renderer.h"
#include "core/defines.h"
#include "core/memory/memory.h"
#include "physics/aabb.h"
#include "batch_renderer.h"
#include "ui/container.h"
#include "ui/color.h"

#define SCA_UI_MAX_NUMBER_LENGTH 10 // u32

struct engine;

typedef struct
{
    color color : 3;
    color background : 3;

    u8 colorIntense : 1;
    u8 colorFaint : 1;
    u8 backgroundIntense : 1;
    u8 backgroundFaint : 1;
    u8 renderBackground : 1;
} text_color;

typedef struct ui_state
{
    memory_pool* pool;
    batch_renderer* renderer;
    container container;
    container prevContainer;

    text_color color;
    u8 stackState : 1;
    u8 defaultContainer : 1;
} ui_state;

void ui_begin(ui_state* state, memory_pool* pool, batch_renderer* renderer);
ui_state* ui_begin_stack(memory_pool* pool, batch_renderer* renderer);
void ui_end(ui_state* state);
void ui_clear(struct engine* e);

void ui_text(ui_state* state, const char* content, u32 length);
void ui_number(ui_state* state, u32 number);

void ui_set_align(ui_state* state, text_align align, u16 xOffset);
void ui_set_position(ui_state* state, text_position position, u16 yOffset);
void ui_set_color(ui_state* state, text_color* color);

void ui_sameline(ui_state* state, bool sameLine);
void ui_feed(ui_state* state);
void ui_nudge(ui_state* state, u32 xOffset);
void ui_space(ui_state* state, u32 yOffset);
void ui_hline(ui_state* state, char lineChar);
void ui_vline(ui_state* state, u32 x, char lineChar);

void ui_switch_container(ui_state* state, container* newContainer);
void ui_restore_container(ui_state* state);

aabb ui_mouse_aabb(struct engine* e);