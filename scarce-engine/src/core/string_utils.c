#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "string_utils.h"

bool is_string_same(const char* lhs, const char* rhs, u32 size)
{
    return strncmp(lhs, rhs, size) == 0;
}

void to_lower(char* string, u32 size)
{
    for(u32 i = 0; i < size; ++i)
    {
        char currentChar = string[i];
        if (currentChar == '\0')
            return;

        string[i] = tolower(currentChar);
    }
}

char* strdup(const char* string)
{
    char* destination = malloc(strlen (string) + 1);
    if (destination == NULL) 
        return NULL;

    strcpy(destination, string);
    return destination;
}