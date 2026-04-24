#include "random.h"
#include <stdlib.h>
#include <time.h>

void random_init()
{
    srand((unsigned int)time(NULL));
}

i32 random_int(i32 min, i32 max)
{
    return min + rand() % (max - min + 1);
}

f32 random_float(f32 min, f32 max)
{
    f32 scale = (f32)rand() / (f32)RAND_MAX; 
    return min + scale * (max - min);
}