#pragma once
#include "platform/platform.h"

#include <X11/X.h>
#include <X11/Xlib.h>

typedef struct x11_window
{
    window_resize_callback resizeCallback;

    Atom wm_delete_window;
    Visual* visual;
    Display* display;
    Window window;
    void* userPointer;

    bool keyPressed[SCA_KEY_COUNT + 1];

    i32 depth;
    i32 width;
    i32 height;
    i32 mouseY;
    i32 mouseX;

    bool leftButtonPress;
    bool rightButtonPress;
    bool middleButtonPress;
    bool isOpen;
} x11_window;