#pragma once
#include <stdint.h>
#include <stdbool.h>

#define KB          1000
#define KiB         1024
#define TO_KB(x)    ((x) * KB)
#define TO_KiB(x)   ((x) * KiB)

#ifndef NULL
    #define NULL 0
#endif

// OS:
#ifdef __linux__
    #define SCA_LINUX
#else
    #error Unsupported system
#endif

// Compiler:
#if defined(__GNUC__) || defined(__clang__)
    #define SECTION_ENTRY __attribute__((section(".text.entry")))
#elif defined(_MSC_VER)
    #pragma section(".text$entry", read, execute)
    #define SECTION_ENTRY __declspec(allocate(".text$entry"))
#else
    #error Unsupported compiler
#endif

typedef  int32_t i32;
typedef  int64_t i64;

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint64_t u64;
typedef uint8_t   u8;

typedef float   f32;
typedef double  f64;

// ANSI escape sequences
#define SCA_RED             "\x1b[31m"
#define SCA_GREEN           "\x1b[32m"
#define SCA_YELLOW          "\x1b[33m"
#define SCA_BLUE            "\x1b[34m"
#define SCA_MAGENTA         "\x1b[35m"
#define SCA_CYAN            "\x1b[36m"
#define SCA_WHITE           "\x1b[37m"
#define SCA_BOLD            "\x1b[1m"
#define SCA_RESET           "\x1b[0m"
#define SCA_CLEAR           "\x1b[2J\x1b[H"

#define SCA_CURSOR_SAVE     "\x1b[s"
#define SCA_CURSOR_RESTORE  "\x1b[u"
#define SCA_CURSOR_HOME     "\x1b[H"

#define SCA_CURSOR_HIDE     "\x1b[?25l"
#define SCA_CURSOR_SHOW     "\x1b[?25h"

#define SCA_CLEAR_LINE      "\x1b[2K"
#define SCA_CLEAR_ALL       "\x1b[2J"