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

        case SCA_KEY_W: keyCode = GLFW_KEY_W; break;
        case SCA_KEY_A: keyCode = GLFW_KEY_A; break;
        case SCA_KEY_S: keyCode = GLFW_KEY_S; break;
        case SCA_KEY_D: keyCode = GLFW_KEY_D; break;
    }

    if (keyCode == -1)
        return false;

    return glfwGetKey(window, keyCode);
}