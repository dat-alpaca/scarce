#pragma once
#include <stdbool.h>
#include "defines.h"
#include "dynamic_array.h"

#define SCA_MAX_U8_LENGTH 3

bool is_string_same(const char* lhs, const char* rhs, u32 size);

bool is_string_number(const char* string, u32 size);
u32 partial_string_number(const char* string, u32 size);

void to_lower(char* string, u32 size);

char* strdup(const char* string);

void number_to_buffer(u8 number, dynamic_array* buffer);
u8 number_to_array(u8 number, char array[SCA_MAX_U8_LENGTH]);

void remove_trailing_space(dynamic_array* buffer);