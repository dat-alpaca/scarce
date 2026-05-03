#pragma once
#include "core/defines.h"
#include "key.h"

/* Core */
#define invalid_handle ((u8)(-1))
void breakpoint();

/* Time */
u64 platform_timestamp_ns();

/* File */
typedef enum
{
    SCA_FILE_NONE   = 0,
    SCA_FILE_READ   = 1 << 0,
    SCA_FILE_WRITE  = 1 << 1,
    SCA_FILE_CREATE = 1 << 2,
} file_mode;

typedef u32 file_descriptor;
#define invalid_file_descriptor (0xFFFFFFFF)

file_descriptor platform_open_file(const char* filepath, file_mode modeFlags);
void platform_read_file(file_descriptor fileDescriptor, void* buffer, u32 length);
void platform_write_file(file_descriptor fileDescriptor, void* buffer, u32 length);
bool platform_is_file_open(file_descriptor fileDescriptor);
u32 platform_file_size(file_descriptor fileDescriptor);
void platform_close_file(file_descriptor fileDescriptor);

/* Memory */
typedef enum
{
    PROTECTION_NONE     = 0,
    PROTECTION_READ     = 1 << 0,
    PROTECTION_WRITE    = 1 << 1,
    PROTECTION_EXECUTE  = 1 << 2
} protection_mode;

typedef enum
{
    MEMORY_NONE         = 0,
    MEMORY_SHARED       = 1 << 0,
    MEMORY_PRIVATE      = 1 << 1,
    MEMORY_ANON         = 1 << 2
} memory_flags;

void* platform_mmap(void *address, u32 length, protection_mode protection, memory_flags flags, file_descriptor fileDescriptor, i64 offset);
void platform_munmmap(void *address, u32 length);
u64 platform_page_size();

/* Window */
typedef void* window_handle;
typedef void (*window_resize_callback)(window_handle*, i32, i32);

window_handle window_init(const char* title, u32 width, u32 height);
bool window_is_key_pressed(window_handle* handle, key key);
bool window_is_open(window_handle* handle);
void window_poll_events(window_handle*);
void window_swap_buffers(window_handle* handle);
void window_get_mouse_position(window_handle*, double* x, double* y);

void* window_get_user_pointer(window_handle* handle);
void  window_set_user_pointer(window_handle* handle, void* pointer);

void window_set_resize_callback(window_handle* handle, window_resize_callback callback);

u64 window_get_time(window_handle*);