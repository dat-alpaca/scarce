#pragma once
#include "batch_renderer.h"
#include "logging/logger.h"
#include "platform/platform.h"
#include "core/memory/memory.h"
#include "core/memory/stack.h"
#include "core/defines.h"

#include "ui/button.h"
#include "ui/text_box.h"

#include "core/physics/aabb.h"
#include "core/view/view.h"

#include "graphics/batch_renderer.h"
#include "ui/ui.h"

typedef struct engine
{
    window_handle* window;
    void* baseAddress;
    u32 memoryPoolSize;
    bool requestExit;

    // AABB:
    bool (*aabb_check_collision)(aabb a, aabb b);

    // Memory:
    void* (*scarce_read)(memory_pool*, u32 address);
    void  (*scarce_write)(memory_pool*, u32 address, u8 value);
    void  (*scarce_write_u32)(memory_pool*, u32 address, u32 value);
    void  (*scarce_copy)(memory_pool*, u32 address, void* buffer, u8 size);

    scarce_push_func scarce_push;
    scarce_pop_func scarce_pop;

    // Random:
    i32 (*random_int)(i32 min, i32 max);
    f32 (*random_float)(f32 min, f32 max);

    // Time:
    u64 (*get_timestamp_ns)();

    // Files:
    file_descriptor (*platform_open_file)(const char*, file_mode);
    bool (*platform_read_file)(file_descriptor, void*, u32);
    void (*platform_write_file)(file_descriptor, void*, u32);
    bool (*platform_is_file_open)(file_descriptor);
    u32 (*platform_file_size)(file_descriptor);
    void (*platform_close_file)(file_descriptor);

    // Input:
    bool (*is_key_pressed)(window_handle*, key key);
    bool (*is_mouse_btn_pressed)(window_handle* handle, mouse_button button);

    // View:
    view_holder* viewHolder;
    void (*view_holder_init)(view_holder* holder, u32 capacity);
    void (*view_holder_destroy)(view_holder* holder);
    void (*view_holder_register)(view_holder* holder, view_data* data);

    void (*view_holder_update)(view_holder* holder, struct engine* e, memory_pool* pool);
    void (*view_holder_render)(view_holder* holder, struct engine* e, memory_pool* pool);

    void (*view_holder_switch_view)(view_holder* holder, struct engine* e, memory_pool* pool, view_id id);
    view_data* (*view_holder_current)(view_holder* holder);

    // Rendering:
    batch_renderer* renderer;
    void (*renderer_set_scale)(batch_renderer* renderer, u32 scale);
    void (*renderer_zero_buffer)(batch_renderer* renderer);
    batch_renderer_cell* (*renderer_get_cell)(batch_renderer* renderer, u32 x, u32 y);
    void (*renderer_set_cell)(batch_renderer* renderer, batch_renderer_cell* cell, u32 x, u32 y);
    void (*renderer_get_mouse_grid_position)(window_handle* window, batch_renderer* renderer, u32* x, u32* y);
    void (*renderer_get_mouse_position)(window_handle* windowHandle, batch_renderer* renderer, u32* x, u32* y);

    // Logging:
    logger* logger;
    void (*log_info)(logger*, const char* message, u32 length);
    void (*log_warn)(logger*, const char* message, u32 length);
    void (*log_error)(logger*, const char* message, u32 length);
    void (*log_critical)(logger*, const char* message, u32 length);
    
    void (*logger_set_header)(logger*, bool state);

    // UI:
    void (*ui_begin)(ui_state* state, memory_pool* pool, batch_renderer* renderer);
    ui_state* (*ui_begin_stack)(memory_pool* pool, batch_renderer* renderer);
    void (*ui_end)(ui_state* state);
    void (*ui_clear)(struct engine* e);

    void (*ui_text)(ui_state* state, const char* content, u32 length);
    void (*ui_number)(ui_state* state, u32 number);
    void (*ui_text_absolute)(ui_state* state, u32 x, u32 y, const char* content, u32 length);

    void (*ui_set_align)(ui_state* state, text_align align, u16 xOffset);
    void (*ui_set_position)(ui_state* state, text_position position, u16 yOffset);
    void (*ui_set_color)(ui_state* state, text_color* color);

    void (*ui_sameline)(ui_state* state, bool sameLine);
    void (*ui_feed)(ui_state* state);
    void (*ui_nudge)(ui_state* state, u32 xOffset);
    void (*ui_space)(ui_state* state, u32 yOffset);
    void (*ui_hline)(ui_state* state, char lineChar);
    void (*ui_vline)(ui_state* state, u32 x, char lineChar);

    aabb (*ui_mouse_aabb)(struct engine* e);

    // UI Button:
    void (*ui_button_init)(ui_button* button, ui_button_callback callback, text_color* color, text_color* hoveredColor, u8 width);
    void (*ui_button_render)(ui_button* button, ui_state* state, const char* content);
    void (*ui_button_update)(ui_button* button, ui_state* state, struct engine* e);

    aabb (*ui_button_aabb)(ui_button* button);

    // UI Text box:
    void (*ui_text_box_init)(ui_text_box* textBox, char* contents, text_color* color, text_color* hoveredColor, u8 width);
    void (*ui_text_box_render)(ui_text_box* textBox, ui_state* state);
    void (*ui_text_box_update)(ui_text_box* textBox, ui_state* state, struct engine* e);

    aabb (*ui_text_box_aabb)(ui_text_box* textBox);

    // UI HSML:
    void (*ui_hsml)(ui_state* state, const char* filepath);
} engine;