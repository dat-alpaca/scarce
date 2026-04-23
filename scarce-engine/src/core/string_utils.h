#pragma once
#include <stdbool.h>
#include "defines.h"

bool is_string_same(const char* lhs, const char* rhs, u32 size);

void to_lower(char* string, u32 size);

char* strdup(const char* string);