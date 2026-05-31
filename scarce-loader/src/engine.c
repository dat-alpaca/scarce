#include "assets/asset_library.h"
#include "assets/asset_upload.h"

#include "memory/memory_system.h"
#include "random.h"
#include "scarce.h"

#include "config/config.h"
#include "loader.h"
#include "ui/hsml/hsml.h"

engine gEngine =
{
    // Memory:
    .scarce_push = scarce_push,
    .scarce_pop = scarce_pop,

    // Files:
    .platform_open_file = platform_open_file,
    .platform_read_file = platform_read_file,
    .platform_write_file = platform_write_file,
    .platform_is_file_open = platform_is_file_open,
    .platform_file_seek = platform_file_seek,
    .platform_file_size = platform_file_size,
    .platform_close_file = platform_close_file,

    // View:
    .viewHolder = NULL,
    .view_holder_register = view_holder_register,
    .view_holder_update = view_holder_update,
    .view_holder_render = view_holder_render,
    .view_holder_switch_view = view_holder_switch_view,
    .view_holder_current = view_holder_current,

    // Time:
    .get_timestamp_ns = platform_timestamp_ns,

    // Random:
    .random_int = random_int,
    .random_float = random_float,

    // Input:
    .is_key_pressed = window_is_key_pressed,
    .is_mouse_btn_pressed = window_is_mouse_btn_pressed,

    // Rendering:
    .renderer_set_scale = batch_renderer_set_scale,
    .renderer_zero_buffer = batch_renderer_zero_buffer,
    .renderer_get_cell = batch_renderer_get_cell,
    .renderer_set_cell = batch_renderer_set_cell,
    .renderer_get_mouse_grid_position = batch_renderer_get_mouse_grid_position,

    // Logging:
    .log_info = log_info,
    .log_warn = log_warn,
    .log_error = log_error,
    .log_critical = log_critical,
    .logger_set_header = logger_set_header,

    // AABB:
    .aabb_check_collision = aabb_check_collision,

    // UI:
    .ui_begin = ui_begin,
    .ui_begin_stack = ui_begin_stack,
    .ui_end = ui_end,
    .ui_clear = ui_clear,

    .ui_text = ui_text,
    .ui_number = ui_number,

    .ui_set_align = ui_set_align,
    .ui_set_position = ui_set_position,
    .ui_set_color = ui_set_color,

    .ui_sameline = ui_sameline,
    .ui_feed = ui_feed,
    .ui_nudge = ui_nudge,
    .ui_space = ui_space,
    .ui_hline = ui_hline,
    .ui_vline = ui_vline,

    .ui_mouse_aabb = ui_mouse_aabb,
    
    // UI Button:
    .ui_button_init = ui_button_init,
    .ui_button_render = ui_button_render,
    .ui_button_update = ui_button_update,
    .ui_button_aabb = ui_button_aabb,

    // UI Textbox:
    .ui_text_box_init = ui_text_box_init,
    .ui_text_box_render = ui_text_box_render,
    .ui_text_box_update = ui_text_box_update,
    .ui_text_box_aabb = ui_text_box_aabb,

    // UI HSML:
    .ui_hsml = ui_hsml,
};