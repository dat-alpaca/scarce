#include <stdbool.h>
#include <stdio.h>

#include "window.h"

GLFWwindow* window = NULL;

bool window_init(const char* title, u32 width, u32 height)
{
    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window)
        return false;

    glfwMakeContextCurrent(window);
    glewInit();

    return true;
}

bool window_is_key_pressed(key key)
{
    int keyCode = -1;
    switch(key)
    {
        case SCA_KEY_0: keyCode = GLFW_KEY_0; break;
        case SCA_KEY_1: keyCode = GLFW_KEY_1; break;
        case SCA_KEY_2: keyCode = GLFW_KEY_2; break;
        case SCA_KEY_3: keyCode = GLFW_KEY_3; break;
        case SCA_KEY_4: keyCode = GLFW_KEY_4; break;
        case SCA_KEY_5: keyCode = GLFW_KEY_6; break;
        case SCA_KEY_6: keyCode = GLFW_KEY_7; break;
        case SCA_KEY_7: keyCode = GLFW_KEY_7; break;
        case SCA_KEY_8: keyCode = GLFW_KEY_8; break;
        case SCA_KEY_9: keyCode = GLFW_KEY_9; break;

        case SCA_KEY_A: keyCode = GLFW_KEY_A; break;
        case SCA_KEY_B: keyCode = GLFW_KEY_B; break;
        case SCA_KEY_C: keyCode = GLFW_KEY_C; break;
        case SCA_KEY_D: keyCode = GLFW_KEY_D; break;
        case SCA_KEY_E: keyCode = GLFW_KEY_E; break;
        case SCA_KEY_F: keyCode = GLFW_KEY_F; break;
        case SCA_KEY_G: keyCode = GLFW_KEY_G; break;
        case SCA_KEY_H: keyCode = GLFW_KEY_H; break;
        case SCA_KEY_I: keyCode = GLFW_KEY_I; break;
        case SCA_KEY_J: keyCode = GLFW_KEY_J; break;
        case SCA_KEY_K: keyCode = GLFW_KEY_K; break;
        case SCA_KEY_L: keyCode = GLFW_KEY_L; break;
        case SCA_KEY_M: keyCode = GLFW_KEY_M; break;
        case SCA_KEY_N: keyCode = GLFW_KEY_N; break;
        case SCA_KEY_O: keyCode = GLFW_KEY_O; break;
        case SCA_KEY_P: keyCode = GLFW_KEY_P; break;
        case SCA_KEY_Q: keyCode = GLFW_KEY_Q; break;
        case SCA_KEY_R: keyCode = GLFW_KEY_R; break;
        case SCA_KEY_S: keyCode = GLFW_KEY_S; break;
        case SCA_KEY_T: keyCode = GLFW_KEY_T; break;
        case SCA_KEY_U: keyCode = GLFW_KEY_U; break;
        case SCA_KEY_V: keyCode = GLFW_KEY_V; break;
        case SCA_KEY_W: keyCode = GLFW_KEY_W; break;
        case SCA_KEY_X: keyCode = GLFW_KEY_X; break;
        case SCA_KEY_Y: keyCode = GLFW_KEY_Y; break;
        case SCA_KEY_Z: keyCode = GLFW_KEY_Z; break;

        case SCA_KEY_KP_0: keyCode = GLFW_KEY_KP_0; break;
        case SCA_KEY_KP_1: keyCode = GLFW_KEY_KP_1; break;
        case SCA_KEY_KP_2: keyCode = GLFW_KEY_KP_2; break;
        case SCA_KEY_KP_3: keyCode = GLFW_KEY_KP_3; break;
        case SCA_KEY_KP_4: keyCode = GLFW_KEY_KP_4; break;
        case SCA_KEY_KP_5: keyCode = GLFW_KEY_KP_5; break;
        case SCA_KEY_KP_6: keyCode = GLFW_KEY_KP_6; break;
        case SCA_KEY_KP_7: keyCode = GLFW_KEY_KP_7; break;
        case SCA_KEY_KP_8: keyCode = GLFW_KEY_KP_8; break;
        case SCA_KEY_KP_9: keyCode = GLFW_KEY_KP_9; break;
        case SCA_KEY_KP_DIVIDE: keyCode = GLFW_KEY_KP_DIVIDE; break;
        case SCA_KEY_KP_MULTIPLY: keyCode = GLFW_KEY_KP_MULTIPLY; break;
        case SCA_KEY_KP_SUBTRACT: keyCode = GLFW_KEY_KP_SUBTRACT; break;
        case SCA_KEY_KP_ADD: keyCode = GLFW_KEY_KP_ADD; break;
        case SCA_KEY_KP_ENTER: keyCode = GLFW_KEY_KP_ENTER; break;
        case SCA_KEY_KP_DECIMAL: keyCode = GLFW_KEY_KP_DECIMAL; break;

        case SCA_KEY_ESC: keyCode = GLFW_KEY_ESCAPE; break;
        case SCA_KEY_F1: keyCode = GLFW_KEY_F1; break;
        case SCA_KEY_F2: keyCode = GLFW_KEY_F2; break;
        case SCA_KEY_F3: keyCode = GLFW_KEY_F3; break;
        case SCA_KEY_F4: keyCode = GLFW_KEY_F4; break;
        case SCA_KEY_F5: keyCode = GLFW_KEY_F5; break;
        case SCA_KEY_F6: keyCode = GLFW_KEY_F6; break;
        case SCA_KEY_F7: keyCode = GLFW_KEY_F7; break;
        case SCA_KEY_F8: keyCode = GLFW_KEY_F8; break;
        case SCA_KEY_F9: keyCode = GLFW_KEY_F9; break;
        case SCA_KEY_F10: keyCode = GLFW_KEY_F10; break;
        case SCA_KEY_F11: keyCode = GLFW_KEY_F11; break;
        case SCA_KEY_F12: keyCode = GLFW_KEY_F12; break;

        case SCA_KEY_UP: keyCode = GLFW_KEY_UP; break;
        case SCA_KEY_DOWN: keyCode = GLFW_KEY_DOWN; break;
        case SCA_KEY_LEFT: keyCode = GLFW_KEY_LEFT; break;
        case SCA_KEY_RIGHT: keyCode = GLFW_KEY_RIGHT; break;

        case SCA_KEY_LSHIFT: keyCode = GLFW_KEY_LEFT_SHIFT; break;
        case SCA_KEY_RSHIFT: keyCode = GLFW_KEY_RIGHT_SHIFT; break;
        case SCA_KEY_LCTRL: keyCode = GLFW_KEY_LEFT_CONTROL; break;
        case SCA_KEY_RCTRL: keyCode = GLFW_KEY_RIGHT_CONTROL; break;
        case SCA_KEY_LALT: keyCode = GLFW_KEY_LEFT_ALT; break;
        case SCA_KEY_RALT: keyCode = GLFW_KEY_RIGHT_ALT; break;
        case SCA_KEY_SPACE: keyCode = GLFW_KEY_SPACE; break;
        case SCA_KEY_ENTER: keyCode = GLFW_KEY_ENTER; break;
        case SCA_KEY_BACKSPACE: keyCode = GLFW_KEY_BACKSPACE; break;
        case SCA_KEY_TAB: keyCode = GLFW_KEY_TAB; break;
        case SCA_KEY_CAPSLOCK: keyCode = GLFW_KEY_CAPS_LOCK; break;

        case SCA_KEY_INS: keyCode = SCA_KEY_INS; break;
        case SCA_KEY_DEL: keyCode = SCA_KEY_DEL; break;
        case SCA_KEY_HOME: keyCode = SCA_KEY_HOME; break;
        case SCA_KEY_END: keyCode = SCA_KEY_END; break;
        case SCA_KEY_PAGEUP: keyCode = SCA_KEY_PAGEUP; break;
        case SCA_KEY_PAGEDOWN: keyCode = SCA_KEY_PAGEDOWN; break;
    }

    if (keyCode == -1)
        return false;

    return glfwGetKey(window, keyCode);
}