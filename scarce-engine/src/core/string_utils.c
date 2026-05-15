#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "string_utils.h"
#include "dynamic_array.h"

bool is_string_same(const char* lhs, const char* rhs, u32 size)
{
    return strncmp(lhs, rhs, size) == 0;
}

bool is_string_number(const char* string, u32 size)
{
    bool result = true;
    for (u32 i = 0; i < size; ++i)
        result &= (bool)isdigit(string[i]);

    return result;
}
u32 partial_string_number(const char* string, u32 size)
{
    u32 validDigits = 0;
    for (u32 i = 0; i < size; ++i)
    {
        if (isdigit(string[i]))
            ++validDigits;
    }

    return validDigits;
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

void number_to_buffer(u64 number, dynamic_array* buffer)
{
    if (number == 0)
    {
        char zero = '0';   
        dynamic_array_push(buffer, &zero, 1);
        return;
    }

    char temp[SCA_MAX_U64_LENGTH];
    u64 length = number_to_array(number, temp);

    for (i8 i = length - 1; i >=0; --i)
        dynamic_array_push(buffer, &temp[i], 1);
}
u64 number_to_array(u64 number, char array[SCA_MAX_U64_LENGTH])
{
    u32 length = 0;
    while (number > 0)
    {
        array[length] = (number % 10) + '0';
        number /= 10;
        ++length;
    }
    return length;
}

void remove_trailing_space(dynamic_array* buffer)
{
    while (true)
    {
        char top = *(char*)&buffer->buffer[dynamic_array_size(buffer) - 1];
        if (top != ' ')
            break;
        dynamic_array_pop(buffer, 1);
    }
}