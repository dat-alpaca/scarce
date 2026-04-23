#pragma once
#include "core/events.h"
#include "core/defines.h"
#include <stdbool.h>

#define GLEW_NO_GLU
#include <GL/glew.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

extern GLFWwindow* window;

bool window_init(const char* title, u32 width, u32 height);
bool window_is_key_pressed(key key);