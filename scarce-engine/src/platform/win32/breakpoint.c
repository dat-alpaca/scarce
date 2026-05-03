#include <intrin.h>
#include "platform/platform.h"

void platform_breakpoint()
{
    __debugbreak();
}