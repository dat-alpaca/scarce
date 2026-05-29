#include "window.h"

#include "memory/tag.h"
#include "logging/logger.h"
#include "platform/platform.h"
#include "memory/memory_system.h"

#include <GL/glx.h>
#include <assert.h>
#include <stdlib.h>

window_handle window_init()
{
    x11_window* window = sca_allocate(TAG_SYSTEM, NULL, sizeof(x11_window), 1);
    window->resizeCallback = NULL;

    window->display = XOpenDisplay(NULL);
    if (!window->display) 
        log_critical_s("Failed to open X11 display", 27);

    return window;
}

void window_create(window_handle handle, const char* title, u32 minWidth, u32 minHeight)
{
    x11_window* window = (x11_window*)handle;

    assert(window->display);
    assert(window->visual);

    i32 screen = DefaultScreen(window->display);

    XSetWindowAttributes windowAttributes = 
    {
        .colormap = XCreateColormap(window->display, RootWindow(window->display, screen), window->visual, AllocNone),
        .event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | 
                      ButtonReleaseMask | PointerMotionMask | StructureNotifyMask
    };

    window->window = XCreateWindow(window->display, RootWindow(window->display, screen), 
                                   0, 0, minWidth, minHeight, 0, window->depth, InputOutput, 
                                   window->visual, CWColormap | CWEventMask, &windowAttributes);

    // Size:
    XSizeHints* sizeHints = XAllocSizeHints();
    sizeHints->flags = PMinSize;
    sizeHints->min_width = minWidth;
    sizeHints->min_height = minHeight;
    XSetWMNormalHints(window->display, window->window, sizeHints);
    XFree(sizeHints);

    XMapWindow(window->display, window->window);

    // Name:
    XStoreName(window->display, window->window, title);
    XSync(window->display, False);
    window->isOpen = true;

    window->wm_delete_window = XInternAtom(window->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(window->display, window->window, &window->wm_delete_window, 1);
}

bool window_is_key_pressed(window_handle handle, key key)
{
    return ((x11_window*)handle)->keyPressed[(u32)key];
}

bool window_is_mouse_btn_pressed(window_handle handle, mouse_button button)
{
    switch(button)
    {
        case SCA_MOUSE_LEFT:
            return ((x11_window*)handle)->leftButtonPress;

        case SCA_MOUSE_RIGHT:
            return ((x11_window*)handle)->rightButtonPress;

        case SCA_MOUSE_MIDDLE:
            return ((x11_window*)handle)->middleButtonPress;
    }

    return false;
}

bool window_is_open(window_handle handle)
{
    return ((x11_window*)handle)->isOpen;
}

void window_poll_events(window_handle handle)
{
    x11_window* window = (x11_window*)handle;

    XEvent event;
    while (XPending(window->display)) 
    {
        XNextEvent(window->display, &event);

        switch (event.type) 
        {
            case ConfigureNotify:
            {
                if (event.xconfigure.width != window->width || event.xconfigure.height != window->height) 
                {
                    window->width = event.xconfigure.width;
                    window->height = event.xconfigure.height;

                    if(window->resizeCallback)
                        window->resizeCallback(handle, window->width, window->height);
                }
            } break;

            case KeyPress:
            case KeyRelease:
            {
                bool keyPressed = (event.type == KeyPress) ? true : false;
                key scanKey;

                KeySym keysym = XLookupKeysym(&event.xkey, 0);
                switch(keysym)
                {
                    case XK_0: scanKey = SCA_KEY_0; break;
                    case XK_1: scanKey = SCA_KEY_1; break;
                    case XK_2: scanKey = SCA_KEY_2; break;
                    case XK_3: scanKey = SCA_KEY_3; break;
                    case XK_4: scanKey = SCA_KEY_4; break;
                    case XK_5: scanKey = SCA_KEY_5; break;
                    case XK_6: scanKey = SCA_KEY_6; break;
                    case XK_7: scanKey = SCA_KEY_7; break;
                    case XK_8: scanKey = SCA_KEY_8; break;
                    case XK_9: scanKey = SCA_KEY_9; break;

                    case XK_a: scanKey = SCA_KEY_A; break;
                    case XK_b: scanKey = SCA_KEY_B; break;
                    case XK_c: scanKey = SCA_KEY_C; break;
                    case XK_d: scanKey = SCA_KEY_D; break;
                    case XK_e: scanKey = SCA_KEY_E; break;
                    case XK_f: scanKey = SCA_KEY_F; break;
                    case XK_g: scanKey = SCA_KEY_G; break;
                    case XK_h: scanKey = SCA_KEY_H; break;
                    case XK_i: scanKey = SCA_KEY_I; break;
                    case XK_j: scanKey = SCA_KEY_J; break;
                    case XK_k: scanKey = SCA_KEY_K; break;
                    case XK_l: scanKey = SCA_KEY_L; break;
                    case XK_m: scanKey = SCA_KEY_M; break;
                    case XK_n: scanKey = SCA_KEY_N; break;
                    case XK_o: scanKey = SCA_KEY_O; break;
                    case XK_p: scanKey = SCA_KEY_P; break;
                    case XK_q: scanKey = SCA_KEY_Q; break;
                    case XK_r: scanKey = SCA_KEY_R; break;
                    case XK_s: scanKey = SCA_KEY_S; break;
                    case XK_t: scanKey = SCA_KEY_T; break;
                    case XK_u: scanKey = SCA_KEY_U; break;
                    case XK_v: scanKey = SCA_KEY_V; break;
                    case XK_w: scanKey = SCA_KEY_W; break;
                    case XK_x: scanKey = SCA_KEY_X; break;
                    case XK_y: scanKey = SCA_KEY_Y; break;
                    case XK_z: scanKey = SCA_KEY_Z; break;

                    case XK_KP_0: scanKey = SCA_KEY_KP_0; break;
                    case XK_KP_1: scanKey = SCA_KEY_KP_1; break;
                    case XK_KP_2: scanKey = SCA_KEY_KP_2; break;
                    case XK_KP_3: scanKey = SCA_KEY_KP_3; break;
                    case XK_KP_4: scanKey = SCA_KEY_KP_4; break;
                    case XK_KP_5: scanKey = SCA_KEY_KP_5; break;
                    case XK_KP_6: scanKey = SCA_KEY_KP_6; break;
                    case XK_KP_7: scanKey = SCA_KEY_KP_7; break;
                    case XK_KP_8: scanKey = SCA_KEY_KP_8; break;
                    case XK_KP_9: scanKey = SCA_KEY_KP_9; break;

                    case XK_KP_Divide: scanKey = SCA_KEY_KP_DIVIDE; break;
                    case XK_KP_Multiply: scanKey = SCA_KEY_KP_MULTIPLY; break;
                    case XK_KP_Subtract: scanKey = SCA_KEY_KP_MULTIPLY; break;
                    case XK_KP_Add: scanKey = SCA_KEY_KP_MULTIPLY; break;
                    case XK_KP_Enter: scanKey = SCA_KEY_KP_MULTIPLY; break;
                    case XK_KP_Decimal: scanKey = SCA_KEY_KP_MULTIPLY; break;

                    case XK_Escape: scanKey = SCA_KEY_ESC; break;
                    
                    case XK_F1 : scanKey = SCA_KEY_F1; break;
                    case XK_F2 : scanKey = SCA_KEY_F2; break;
                    case XK_F3 : scanKey = SCA_KEY_F3; break;
                    case XK_F4 : scanKey = SCA_KEY_F4; break;
                    case XK_F5 : scanKey = SCA_KEY_F5; break;
                    case XK_F6 : scanKey = SCA_KEY_F6; break;
                    case XK_F7 : scanKey = SCA_KEY_F7; break;
                    case XK_F8 : scanKey = SCA_KEY_F8; break;
                    case XK_F9 : scanKey = SCA_KEY_F9; break;
                    case XK_F10: scanKey = SCA_KEY_F10; break;
                    case XK_F11: scanKey = SCA_KEY_F11; break;
                    case XK_F12: scanKey = SCA_KEY_F12; break;

                    case XK_Up: scanKey = SCA_KEY_UP; break;
                    case XK_Down: scanKey = SCA_KEY_DOWN; break;
                    case XK_Left: scanKey = SCA_KEY_LEFT; break;
                    case XK_Right: scanKey = SCA_KEY_RIGHT; break;

                    case XK_Shift_L: scanKey = SCA_KEY_LSHIFT; break;
                    case XK_Shift_R: scanKey = SCA_KEY_RSHIFT; break;
                    case XK_Control_L: scanKey = SCA_KEY_LCTRL; break;
                    case XK_Control_R: scanKey = SCA_KEY_RCTRL; break;
                    case XK_Alt_L: scanKey = SCA_KEY_LALT; break;
                    case XK_Alt_R: scanKey = SCA_KEY_RALT; break;
                    case XK_space: scanKey = SCA_KEY_SPACE; break;
                    case XK_Linefeed: scanKey = SCA_KEY_ENTER; break;
                    case XK_BackSpace: scanKey = SCA_KEY_BACKSPACE; break;
                    case XK_Tab: scanKey = SCA_KEY_TAB; break;
                    case XK_Caps_Lock: scanKey = SCA_KEY_CAPSLOCK; break;

                    case XK_Insert: scanKey = SCA_KEY_INS; break;
                    case XK_Delete: scanKey = SCA_KEY_DEL; break;
                    case XK_Home: scanKey = SCA_KEY_HOME; break;
                    case XK_End: scanKey = SCA_KEY_END; break;
                    case XK_Page_Up: scanKey = SCA_KEY_PAGEUP; break;
                    case XK_Page_Down: scanKey = SCA_KEY_PAGEDOWN; break;
                }

                window->keyPressed[(u32)scanKey] = keyPressed;
            } break;

            case ButtonPress:
            {
                switch(event.xbutton.button)
                {
                    case Button1:
                        window->leftButtonPress = true;
                        break;

                    case Button2:
                        window->rightButtonPress = true;
                        break;

                    case Button3:
                        window->middleButtonPress = true;
                        break;
                }
            } break;

            case ButtonRelease:
            {
                switch(event.xbutton.button)
                {
                    case Button1:
                        window->leftButtonPress = false;
                        break;

                    case Button2:
                        window->rightButtonPress = false;
                        break;

                    case Button3:
                        window->middleButtonPress = false;
                        break;
                }
            } break;

            case MotionNotify:
            {
                window->mouseX = event.xmotion.x;
                window->mouseY = event.xmotion.y;
            } break;
                
            case ClientMessage:
            {
                if ((Atom)event.xclient.data.l[0] == window->wm_delete_window) 
                    window->isOpen = false;
            } break;
        }
    }
}

void window_swap_buffers(window_handle handle)
{
    glXSwapBuffers(((x11_window*)handle)->display, ((x11_window*)handle)->window);
}

void window_get_mouse_position(window_handle handle, double* x, double* y)
{
    *x = (double)((x11_window*)handle)->mouseX;
    *y = (double)((x11_window*)handle)->mouseY;
}

void* window_get_user_pointer(window_handle handle)
{
    return ((x11_window*)handle)->userPointer;
}

void  window_set_user_pointer(window_handle handle, void* pointer)
{
    ((x11_window*)handle)->userPointer = pointer;
}

void window_set_resize_callback(window_handle handle, window_resize_callback callback)
{
    ((x11_window*)handle)->resizeCallback = callback;
}

u64 window_get_time(window_handle _)
{
    /* Implement */
    return 0;
}