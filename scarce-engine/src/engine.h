#pragma once
#include "logging/logger.h"
#include "platform/platform.h"
#include "core/memory/memory.h"
#include "core/memory/stack.h"
#include "core/defines.h"

#include "ui/ui.h"
#include "ui/button.h"
#include "ui/text_box.h"

#include "core/view/view.h"

#include "graphics/text_renderer.h"

typedef struct engine
{
    window_handle* window;
    text_renderer* renderer;
    u32 memoryPoolSize;

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
    void (*platform_read_file)(file_descriptor, void*, u32);
    void (*platform_write_file)(file_descriptor, void*, u32);
    bool (*platform_is_file_open)(file_descriptor);
    u32 (*platform_file_size)(file_descriptor);
    void (*platform_close_file)(file_descriptor);

    // Input:
    bool (*is_key_pressed)(window_handle*, key key);
    bool (*is_mouse_btn_pressed)(window_handle* handle, mouse_button button);

    // View:
    void (*view_holder_init)(view_holder* holder, u32 capacity);
    void (*view_holder_destroy)(view_holder* holder);
    void (*view_holder_register)(view_holder* holder, view_data* data);

    void (*view_holder_switch_view)(view_holder* holder, struct engine* e, memory_pool* pool, view_id id);
    view_data* (*view_holder_current)(view_holder* holder);

    // Rendering:
    void (*renderer_zero_buffer)(text_renderer* renderer);
    void (*renderer_set_character_letter)(text_renderer* renderer, u32 x, u32 y, char letter);
    void (*renderer_set_character_color)(text_renderer* renderer, u32 x, u32 y, float r, float g, float b);
    void (*renderer_set_character_background_color)(text_renderer* renderer, u32 x, u32 y, float r, float g, float b, bool paint);
    void (*renderer_set_character_size)(text_renderer* renderer, u32 characterSize);
    u32  (*renderer_width)(text_renderer* renderer);
    u32  (*renderer_height)(text_renderer* renderer);
    u32  (*renderer_window_width)(text_renderer* renderer);
    u32  (*renderer_window_height)(text_renderer* renderer);
    u32  (*renderer_character_size)(text_renderer* renderer);

    void (*get_mouse_position)(window_handle* windowHandle, text_renderer* renderer, u32* x, u32* y);

    // Logging:
    logger* logger;
    void (*log_info)(logger*, const char* message, u32 length);
    void (*log_warn)(logger*, const char* message, u32 length);
    void (*log_error)(logger*, const char* message, u32 length);
    void (*log_critical)(logger*, const char* message, u32 length);
    
    void (*logger_set_header)(logger*, bool state);

    // UI:
    ui_state* (*ui_begin)(memory_pool* pool, text_renderer* renderer);
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
    void (*ui_hline)(ui_state* state, u32 y, char lineChar);

    // UI Button:
    void (*ui_button_init)(ui_button* button, ui_button_callback callback, text_color* color, text_color* hoveredColor, u8 width);
    void (*ui_button_render)(ui_button* button, ui_state* state, const char* content);
    void (*ui_button_update)(ui_button* button, ui_state* state, struct engine* e);

    // UI Text box::
    void (*ui_text_box_init)(ui_text_box* textBox, char* contents, text_color* color, text_color* hoveredColor, u8 width);
    void (*ui_text_box_render)(ui_text_box* textBox, ui_state* state);
    void (*ui_text_box_update)(ui_text_box* textBox, ui_state* state, struct engine* e);
} engine;