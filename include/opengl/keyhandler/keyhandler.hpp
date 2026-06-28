#ifndef KEYHANDLER_H
#define KEYHANDLER_H

#include "types/types.hpp"
#include <GLFW/glfw3.h>

struct KeyHandler {
    player_ptr player;
    components_vector &components;
    std::mutex &componets_mutex;
    bool can_add_new_pause = true;

    KeyHandler(player_ptr, components_vector &, std::mutex &components_mutex);

    GLFWkeyfun make_key_callback(opengl_context);

    void animate_pause();
};

#endif
