#ifndef KEYHANDLER_H
#define KEYHANDLER_H

#include "types/types.hpp"
#include <GLFW/glfw3.h>
#include <mutex>

struct State {
    bool can_add_new = true;
    bool reset_opacity = false;
};

struct KeyHandler {
    player_ptr player;
    components_vector &components;
    std::mutex &componets_mutex;
    State pause{};
    State forward{};
    State backward{};

    KeyHandler(player_ptr, components_vector &, std::mutex &components_mutex);

    GLFWkeyfun make_key_callback(opengl_context);

    void make_aimation_thread(animated_ptr component, State &);
    void animate(animated_ptr component, State *can_add_new);
};

#endif
