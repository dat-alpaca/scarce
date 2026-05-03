#pragma once
#include <GL/glx.h>

typedef struct window
{
    Display *display;
    Window window;
    GLXContext context;
    Atom wm_delete_window;
    int width, height;
} window;