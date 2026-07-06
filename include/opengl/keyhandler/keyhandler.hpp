#ifndef KEYHANDLER_H
#define KEYHANDLER_H

#include "types/types.hpp"
#include <GLFW/glfw3.h>
#include <atomic>

struct State {
    std::atomic_bool can_add_new = true;
    std::atomic_bool reset_opacity = false;
};

struct KeyHandler {

    KeyHandler(player_ptr, components_container &);

    GLFWkeyfun make_key_callback(opengl_context);

    void make_aimation_thread(State &, animated_ptr component);
    void animate(State *can_add_new, animated_ptr component);

  private:
    components_container &components;
    player_ptr player;
    State pause;
    State backward;
    State forward;
};

#endif
