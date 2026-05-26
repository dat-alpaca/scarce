#include "opengl_rhi.h"
#include "rhi/rhi.h"
#include "platform/linux/window.h"
#include "logging/logger.h"

#define GLEW_NO_GLU
#include <GL/glew.h>
#include "GL/glx.h"

typedef struct opengl_x11_internal
{
    GLXContext context;
} opengl_x11_internal;

void rhi_initialize_window(rhi rhi, window_handle handle)
{
    x11_window* x11 = (x11_window*)handle;
    opengl_rhi* gl = (opengl_rhi*)rhi;
    
    opengl_x11_internal* glX11 = malloc(sizeof(opengl_x11_internal));

    int visualAttribs[] =
    {
        GLX_RENDER_TYPE,   GLX_RGBA_BIT,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_DOUBLEBUFFER,  True,
        GLX_RED_SIZE,      8,
        GLX_GREEN_SIZE,    8,
        GLX_BLUE_SIZE,     8,
        GLX_ALPHA_SIZE,    8,
        GLX_DEPTH_SIZE,    24,
        None
    };

    int fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(x11->display, DefaultScreen(x11->display), visualAttribs, &fbcount);
    if (!fbc)
        log_critical_s("Failed to retrieve FBConfig", 28);

    int contextAttribs[] = 
    {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 6,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = 
        (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
    
    glX11->context = glXCreateContextAttribsARB(x11->display, fbc[0], NULL, True, contextAttribs);
    XFree(fbc);

    if (!glX11->context)
        log_critical_s("Failed to create GLX Context", 27);

    glXMakeCurrent(x11->display, x11->window, glX11->context);

    glewExperimental = GL_TRUE;
    glewInit();

    gl->platformData = glX11;
}

void rhi_swap_buffers(rhi rhi, window_handle handle)
{
    opengl_rhi* _ = (opengl_rhi*)rhi;
    x11_window* window = (x11_window*)handle;

    glXSwapBuffers(window->display, window->window);
}