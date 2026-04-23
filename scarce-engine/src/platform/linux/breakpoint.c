#include "platform/platform.h"

void breakpoint()
{
    __builtin_trap();
}