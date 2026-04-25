#pragma once
#include "platform/platform.h"
#include "core/memory/memory.h"
#include "core/memory/stack.h"
#include "core/defines.h"
#include "core/events.h"

#include "graphics/text_renderer.h"

typedef struct
{
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
    bool (*is_key_pressed)(key key);

    // Rendering:
    text_renderer* renderer;
    void (*renderer_set_character_letter)(text_renderer* renderer, u32 x, u32 y, char letter);
    void (*renderer_set_character_color)(text_renderer* renderer, u32 x, u32 y, float r, float g, float b);
    void (*renderer_set_character_background_color)(text_renderer* renderer, u32 x, u32 y, float r, float g, float b, bool paint);
    void (*renderer_set_character_size)(text_renderer* renderer, u32 characterSize);
    u32  (*renderer_width)(text_renderer* renderer);
    u32  (*renderer_height)(text_renderer* renderer);
    u32  (*renderer_window_width)(text_renderer* renderer);
    u32  (*renderer_window_height)(text_renderer* renderer);
    u32  (*renderer_character_size)(text_renderer* renderer);
} engine;

typedef void (*load_func)(memory_pool*, engine*);
typedef bool (*update_func)(memory_pool*);
typedef void (*unload_func)(memory_pool*);

typedef struct
{
    void (*on_load)(memory_pool*, engine*);
    bool (*on_update)(memory_pool*);
    void (*on_unload)(memory_pool*);
} exports;

