#ifndef KEYHANDLER_H
#define KEYHANDLER_H

#include "types/types.hpp"
#include <GLFW/glfw3.h>
#include <mutex>

struct State {
    animated_ptr component;
    bool can_add_new = true;
    bool reset_opacity = false;

    State(animated_ptr);
};

struct KeyHandler {
    player_ptr player;
    components_vector &components;
    std::mutex &componets_mutex;
    State pause;
    State backward;
    State forward;

    KeyHandler(player_ptr, components_vector &, std::mutex &components_mutex);

    GLFWkeyfun make_key_callback(opengl_context);

    void make_aimation_thread(State &);
    void animate(State *can_add_new);
};

#endif
