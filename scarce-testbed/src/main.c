#include "renderer.h"
#include "timing.h"
#include "utils.h"
#include "map.h"
#include "input.h"

static engine* _e = 0;

void on_load(memory_pool* pool, engine* engine)
{
    _e = engine;

    // Prepare file descriptors:
    file_descriptor* fileDescriptors = (file_descriptor*)(&pool[SCA_FILE_DESCRIPTORS_ADDR]);
    for(u8 i = 0; i < SCA_FILE_DESCRIPTORS_SIZE / sizeof(file_descriptor); ++i)
        *(fileDescriptors + i) = invalid_file_descriptor;

    // Loads world data + initial symbol table
    symbol* data = (symbol*)(&pool[SCA_VRAM_ADDR]);
    for(u32 i = 0; i < (32 * 8); ++i)
    {
        data[i].glyphID = 0x00;
        data[i].color = SY_COLOR_WHITE;
        data[i].faint = 1;
        data[i].backgroundColor = SY_COLOR_NONE;
    }

    pool[SCA_US_SYMBOL_TABLE_ADDR + 0] = '.';
    pool[SCA_US_SYMBOL_TABLE_ADDR + 1] = 'P';
}

bool on_update(memory_pool* pool)
{
    // Delta time:
    calculate_delta_time(_e, pool);
    increment_accumulator_time(_e, pool, SCA_US_FRAME_ACCUM_ADDR);
    
    /* Input */
    player_input* input = (player_input*)&pool[SCA_US_INPUT_ADDR];
    bool moving = input->up || input->down || input->left || input->right;
    {
        input->up    = (_e->is_key_pressed(SCA_KEY_W));
        input->down  = (_e->is_key_pressed(SCA_KEY_S));
        input->left  = (_e->is_key_pressed(SCA_KEY_A));
        input->right = (_e->is_key_pressed(SCA_KEY_D));
        
        if (_e->is_key_pressed(SCA_KEY_0))
            return false;
    }

    u8* walkTimer = (u8*)&pool[SCA_US_WALK_FRAMES_ADDR];
    if (!moving)
       *walkTimer = 0;
    else if (*walkTimer == 0) 
    {
        u8 x = pool[SCA_US_PLAYER_ADDR + 0];
        u8 y = pool[SCA_US_PLAYER_ADDR + 1];

        if(input->up)  
        {
            if (pool[SCA_US_PLAYER_ADDR + 1] - 1 >= 0)
                pool[SCA_US_PLAYER_ADDR + 1] -= 1;
        }
        if(input->down)  
        {
            if (pool[SCA_US_PLAYER_ADDR + 1] + 1 < 8)
                pool[SCA_US_PLAYER_ADDR + 1] += 1;
        }
        if(input->left)  
        {
            if (pool[SCA_US_PLAYER_ADDR + 0] - 1 >= 0)
                pool[SCA_US_PLAYER_ADDR + 0] -= 1;
        }
        if(input->right)  
        {
            if (pool[SCA_US_PLAYER_ADDR + 0] + 1 < 32)
                pool[SCA_US_PLAYER_ADDR + 0] += 1;
        }

        *walkTimer = 1;
    }
    
    /* Render */
    u32* frameAccumulator = (u32*)_e->scarce_read(pool, SCA_US_FRAME_ACCUM_ADDR);
    if(*frameAccumulator > _ONE_FRAME)
    {
        /* Counters */
        if (*walkTimer > 0) 
        {
            *walkTimer += 1;
            if (*walkTimer > _HALF_SECOND / _ONE_FRAME)
                *walkTimer = 0;
        }

        *frameAccumulator = 0;

        // Header:
        render_file(_e, pool, "res/header", SCA_FILE_DESCRIPTORS_ADDR);
        
        // Write world data to VRAM
        symbol* data = (symbol*)(&pool[SCA_VRAM_ADDR]);
        for(u32 i = 0; i < (32 * 8); ++i)
        {
            data[i].glyphID = 0x00;
            data[i].color = SY_COLOR_WHITE;
            data[i].faint = 1;
            data[i].backgroundColor = SY_COLOR_NONE;
        }

        // Write player data to VRAM
        u8 x = pool[SCA_US_PLAYER_ADDR + 0];
        u8 y = pool[SCA_US_PLAYER_ADDR + 1];
        symbol* sy = (symbol*)(&pool[SCA_VRAM_ADDR]);
        sy[32 * y + x].glyphID = 0x1;
        sy[32 * y + x].color = SY_COLOR_RED;
        sy[32 * y + x].faint = 0;
        sy[32 * y + x].isColorIntense = 1;
        sy[32 * y + x].backgroundColor = SY_COLOR_NONE;

        render_vram(_e, pool, SCA_VRAM_ADDR, SCA_VRAM_SIZE, SCA_US_SYMBOL_TABLE_ADDR, 32, 8, 4);
    }
    
    return true;
}

void on_unload(memory_pool* pool)
{
}  

SECTION_ENTRY const exports _ = {
    .on_load   = on_load,
    .on_update = on_update,
    .on_unload = on_unload
};