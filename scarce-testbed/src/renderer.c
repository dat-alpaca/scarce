#include "renderer.h"
#include "core/memory/memory.h"
#include "utils.h"

static float* get_color_with_flags(u32 symbolColor, bool isIntense, bool isFaint)
{
    static float colors[][3] = 
    {
        { 0.141f, 0.161f, 0.180f }, // Black
        { 0.933f, 0.275f, 0.282f }, // Red
        { 0.165f, 0.631f, 0.596f }, // Green
        { 0.961f, 0.686f, 0.208f }, // Yellow
        { 0.231f, 0.447f, 0.702f }, // Blue
        { 0.612f, 0.404f, 0.631f }, // Magenta
        { 0.106f, 0.588f, 0.725f }, // Cyan
        { 0.937f, 0.941f, 0.945f }  // White
    };

    static float colors_intense[][3] = 
    {
        { 0.306f, 0.341f, 0.384f }, // Bright Black
        { 0.996f, 0.380f, 0.380f }, // Bright Red
        { 0.110f, 0.925f, 0.612f }, // Bright Green
        { 0.996f, 0.816f, 0.298f }, // Bright Yellow
        { 0.365f, 0.592f, 0.847f }, // Bright Blue
        { 0.757f, 0.525f, 0.773f }, // Bright Magenta
        { 0.106f, 0.796f, 0.933f }, // Bright Cyan
        { 0.992f, 0.996f, 0.996f }  // Bright White
    };

    static float faint_result[3];

    u32 index = (symbolColor > SY_COLOR_WHITE) ? SY_COLOR_WHITE : symbolColor;

    float* selected = (isIntense) ? colors_intense[index] : colors[index];
    if (isFaint) 
    {
        faint_result[0] = selected[0] * 0.5f;
        faint_result[1] = selected[1] * 0.5f;
        faint_result[2] = selected[2] * 0.5f;
        return faint_result;
    }

    return selected;
}

static void render_character_styles(engine* e, memory_pool* pool, symbol symbol, u32 x, u32 y)
{
    float* color = (float*)e->scarce_push(pool, sizeof(float) * 3);
    color = get_color_with_flags(symbol.color, symbol.isColorIntense, symbol.faint);
    e->renderer_set_character_color
    (
        e->renderer, x, y, 
        color[0], color[1], color[2]
    );

    color = get_color_with_flags(symbol.backgroundColor, symbol.isBackgroundColorIntense, symbol.faint);
    e->renderer_set_character_background_color
    (
        e->renderer, x, y, 
        color[0], color[1], color[2], 
        symbol.backgroundColor != SY_COLOR_NONE
    );

    e->scarce_pop(pool, sizeof(float) * 3);
}

void render_file(engine* e, memory_pool* pool, const char* filepath, u32 fileDescriptorAddr)
{
    u8 descriptorAddr = open_file(e, pool, filepath, SCA_FILE_READ);
    
    u32 x = 0;
    u32 y = 0;
    if (descriptorAddr != invalid_handle)
    {
        file_descriptor* descriptors = (file_descriptor*)(&pool[fileDescriptorAddr]);
        file_descriptor descriptor = descriptors[descriptorAddr];

        while(true)
        {
            e->scarce_push(pool, 1);
            
            e->platform_read_file(descriptor, &pool[SCA_STACK_ADDRESS], 1);
            
            if((char)pool[SCA_STACK_ADDRESS] == EOF)
            {
                e->scarce_pop(pool, 1);
                break;
            }

            if(x + 1 >= e->renderer_width(e->renderer))
            {
                e->scarce_pop(pool, 1);
                ++y;
                x = 0;
                continue;
            }

            if(y + 1 >= e->renderer_height(e->renderer))
            {
                e->scarce_pop(pool, 1);
                break;
            }

            if((char)pool[SCA_STACK_ADDRESS] == '\n')
            {
                e->scarce_pop(pool, 1);
                y++;
                x = 0;
                continue;
            }
            e->renderer_set_character_color(e->renderer, x, y, 1.0f, 1.0f, 1.0f);
            e->renderer_set_character_letter(e->renderer, x, y, (char)pool[SCA_STACK_ADDRESS]);
            x++;

            e->scarce_pop(pool, 1);
        }
        
        descriptors[descriptorAddr] = invalid_file_descriptor;
    }
}

void render_vram(engine* e, memory_pool* pool, u32 vramAddress, u32 vramSizeBytes, u32 symbolTableAddress, u8 width, u8 height, u8 offset)
{
    symbol* data = (symbol*)&pool[vramAddress];
    u32 count = 0;

    for(u32 x = 0; x < 32; ++x)
    {
        for(u32 y = 0; y < 8; ++y)
        {
            char letter = pool[symbolTableAddress + (u8)data[y * 32 + x].glyphID];
            render_character_styles(e, pool, data[y * 32 + x], x, y + offset);
            e->renderer_set_character_letter(e->renderer, x, y + offset, letter);
        }
    }
}