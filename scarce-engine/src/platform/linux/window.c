#include "platform/platform.h"

#define GLEW_NO_GLU
#include <GL/glew.h>
#include <GL/glx.h>
#include <stdlib.h>

typedef struct x11_window
{
    window_resize_callback resizeCallback;

    Atom wm_delete_window;
    GLXContext context;
    Display* display;
    Window window;
    void* userPointer;

    bool keyPressed[SCA_KEY_COUNT + 1];

    i32 width;
    i32 height;
    i32 mouseY;
    i32 mouseX;

    bool leftButtonPress;
    bool rightButtonPress;
    bool middleButtonPress;
    bool isOpen;
} x11_window;

window_handle window_init(const char* title, u32 minWidth, u32 minHeight)
{
    x11_window* window = malloc(sizeof(x11_window));
    // platform_mmap(window, sizeof(x11_window), PROTECTION_READ | PROTECTION_WRITE, 
    //               MEMORY_ANON | MEMORY_PRIVATE, invalid_file_descriptor, 0);

    window->resizeCallback = NULL;

    window->display = XOpenDisplay(NULL);
    if (!window->display) 
    {
        // TODO: Failed to open X display
    }

    static int visual_attribs[] = 
    {
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_DOUBLEBUFFER, True,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        None
    };

    int framebufferCount;
    GLXFBConfig* fbc = glXChooseFBConfig(window->display, DefaultScreen(window->display), visual_attribs, &framebufferCount);
    XVisualInfo* vi = glXGetVisualFromFBConfig(window->display, fbc[0]);

    XSetWindowAttributes swa = 
    {
        .colormap = XCreateColormap(window->display, RootWindow(window->display, vi->screen), vi->visual, AllocNone),
        
        .event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | 
                      ButtonReleaseMask | PointerMotionMask | StructureNotifyMask
    };

    XSizeHints* sizeHints = XAllocSizeHints();
    sizeHints->flags = PMinSize;
    sizeHints->min_width = minWidth;
    sizeHints->min_height = minHeight;
    XSetWMNormalHints(window->display, window->window, sizeHints);
    XFree(sizeHints);

    window->window = XCreateWindow(window->display, RootWindow(window->display, vi->screen), 
                                   0, 0, minWidth, minHeight, 0, vi->depth, InputOutput, 
                                   vi->visual, CWColormap | CWEventMask, &swa);

    XMapWindow(window->display, window->window);
    XStoreName(window->display, window->window, title);
    window->isOpen = true;

    // Context
    int context_attribs[] = 
    {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 6,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = 
        (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
    
    window->context = glXCreateContextAttribsARB(window->display, fbc[0], NULL, True, context_attribs);
    glXMakeCurrent(window->display, window->window, window->context);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) 
    {
        // TODO: GLEW init failed
    }

    window->wm_delete_window = XInternAtom(window->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(window->display, window->window, &window->wm_delete_window, 1);

    return window;
}

bool window_is_key_pressed(window_handle* handle, key key)
{
    return ((x11_window*)handle)->keyPressed[(u32)key];
}

bool window_is_mouse_btn_pressed(window_handle* handle, mouse_button button)
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

bool window_is_open(window_handle* handle)
{
    return ((x11_window*)handle)->isOpen;
}

void window_poll_events(window_handle* handle)
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

                    case XK_A: scanKey = SCA_KEY_A; break;
                    case XK_B: scanKey = SCA_KEY_B; break;
                    case XK_C: scanKey = SCA_KEY_C; break;
                    case XK_D: scanKey = SCA_KEY_D; break;
                    case XK_E: scanKey = SCA_KEY_E; break;
                    case XK_F: scanKey = SCA_KEY_F; break;
                    case XK_G: scanKey = SCA_KEY_G; break;
                    case XK_H: scanKey = SCA_KEY_H; break;
                    case XK_I: scanKey = SCA_KEY_I; break;
                    case XK_J: scanKey = SCA_KEY_J; break;
                    case XK_K: scanKey = SCA_KEY_K; break;
                    case XK_L: scanKey = SCA_KEY_L; break;
                    case XK_M: scanKey = SCA_KEY_M; break;
                    case XK_N: scanKey = SCA_KEY_N; break;
                    case XK_O: scanKey = SCA_KEY_O; break;
                    case XK_P: scanKey = SCA_KEY_P; break;
                    case XK_Q: scanKey = SCA_KEY_Q; break;
                    case XK_R: scanKey = SCA_KEY_R; break;
                    case XK_S: scanKey = SCA_KEY_S; break;
                    case XK_T: scanKey = SCA_KEY_T; break;
                    case XK_U: scanKey = SCA_KEY_U; break;
                    case XK_V: scanKey = SCA_KEY_V; break;
                    case XK_W: scanKey = SCA_KEY_W; break;
                    case XK_X: scanKey = SCA_KEY_X; break;
                    case XK_Y: scanKey = SCA_KEY_Y; break;
                    case XK_Z: scanKey = SCA_KEY_Z; break;

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

void window_swap_buffers(window_handle* handle)
{
    glXSwapBuffers(((x11_window*)handle)->display, ((x11_window*)handle)->window);
}

void window_get_mouse_position(window_handle* handle, double* x, double* y)
{
    *x = (double)((x11_window*)handle)->mouseX;
    *y = (double)((x11_window*)handle)->mouseY;
}

void* window_get_user_pointer(window_handle* handle)
{
    return ((x11_window*)handle)->userPointer;
}

void  window_set_user_pointer(window_handle* handle, void* pointer)
{
    ((x11_window*)handle)->userPointer = pointer;
}

void window_set_resize_callback(window_handle* handle, window_resize_callback callback)
{
    ((x11_window*)handle)->resizeCallback = callback;
}

u64 window_get_time(window_handle* _)
{
    /* Implement */
    return 0;
}