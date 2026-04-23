#pragma once
#include "core/memory/memory.h"

/* Stack */
#ifndef SCA_STACK_SIZE
    #define SCA_STACK_SIZE              (64)
#endif

/* File descriptors */
#define SCA_FILE_DESCRIPTORS_ADDR       (SCA_STACK_SIZE_ADDRESS + SCA_STACK_SIZE)
#define SCA_FILE_DESCRIPTORS_SIZE       (16)

/* VRAM */
#define SCA_VRAM_ADDR                   (SCA_FILE_DESCRIPTORS_ADDR + SCA_FILE_DESCRIPTORS_SIZE)
#define SCA_VRAM_SIZE                   (512)

/* User space */
#define SCA_USER_SPACE_ADDRESS          (SCA_VRAM_ADDR + SCA_VRAM_SIZE)

#define SCA_US_LAST_TIME_ADDR           (SCA_USER_SPACE_ADDRESS)
    #define SCA_US_LAST_TIME_SIZE       (8)

#define SCA_US_DELTA_TIME_ADDR          (SCA_US_LAST_TIME_ADDR + SCA_US_LAST_TIME_SIZE)
    #define SCA_US_DELTA_TIME_SIZE      (4)

#define SCA_US_FRAME_ACCUM_ADDR         (SCA_US_DELTA_TIME_ADDR + SCA_US_DELTA_TIME_SIZE)
    #define SCA_US_FRAME_ACCUM_SIZE     (4)

#define SCA_US_SYMBOL_TABLE_ADDR        (SCA_US_FRAME_ACCUM_ADDR + SCA_US_FRAME_ACCUM_SIZE)
    #define SCA_US_SYMBOL_TABLE_SIZE    (16)

/* Input */
#define SCA_US_INPUT_ADDR               (SCA_US_SYMBOL_TABLE_ADDR + SCA_US_SYMBOL_TABLE_SIZE)
    #define SCA_US_INPUT_SIZE           (1)

// Player:
#define SCA_US_WALK_FRAMES_ADDR         (SCA_US_INPUT_ADDR + SCA_US_INPUT_SIZE)
    #define SCA_US_WALK_FRAMES_SIZE     (1)

#define SCA_US_PLAYER_ADDR              (SCA_US_WALK_FRAMES_ADDR + SCA_US_WALK_FRAMES_SIZE)
    #define SCA_US_PLAYER_SIZE          (2)

