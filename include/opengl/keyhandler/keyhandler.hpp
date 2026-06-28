#ifndef KEYHANDLER_H
#define KEYHANDLER_H

#include "types/types.hpp"
#include <GLFW/glfw3.h>

GLFWkeyfun make_key_callback(std::shared_ptr<Context::OpenGL>, player_ptr);

#endif
