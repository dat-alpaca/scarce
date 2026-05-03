#include "platform/platform.h"

void platform_breakpoint()
{
    __builtin_trap();
}