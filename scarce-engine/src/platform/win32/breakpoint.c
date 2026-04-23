#include <intrin.h>
#include "platform/platform.h"

void breakpoint()
{
    __debugbreak();
}