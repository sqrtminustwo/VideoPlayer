#include <thread>
#include "opengl/context/context.hpp" // IWYU pragma: keep
#include "ffmpeg/video_player.hpp"    // IWYU pragma: keep
#include "opengl/keyhandler/keyhandler.hpp"
#include "opengl/drawers/overlay/components/animated/animated.hpp"

KeyHandler::KeyHandler(player_ptr player, components_container &components)
    : player{player}, components{components} {}

GLFWkeyfun KeyHandler::make_key_callback(opengl_context opengl_context) {
    glfwSetWindowUserPointer(opengl_context->window, this);
    return [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        KeyHandler *keyhandler = (KeyHandler *)glfwGetWindowUserPointer(window);
        auto player = keyhandler->player;
        auto p = action == GLFW_PRESS;
        auto r_or_p = action == GLFW_REPEAT || p;
        if (key == GLFW_KEY_SPACE && p) {
            keyhandler->make_aimation_thread(
                keyhandler->pause,
                std::dynamic_pointer_cast<Overlay::Animated>(keyhandler->components[PAUSE])
            );
            player->pause.toggle();
        }
        if (key == GLFW_KEY_LEFT && r_or_p) {
            keyhandler->make_aimation_thread(
                keyhandler->backward,
                std::dynamic_pointer_cast<Overlay::Animated>(keyhandler->components[BACKWARD])
            );
            player->skip_seconds_forward(false);
        }
        if (key == GLFW_KEY_RIGHT && r_or_p) {
            keyhandler->make_aimation_thread(
                keyhandler->forward,
                std::dynamic_pointer_cast<Overlay::Animated>(keyhandler->components[FORWARD])
            );
            player->skip_seconds_forward(true);
        }
    };
}

void KeyHandler::make_aimation_thread(State &state, animated_ptr component) {
    if (state.can_add_new) {
        std::thread t(&KeyHandler::animate, this, &state, component);
        t.detach();
    } else state.reset_opacity = true;
}

void KeyHandler::animate(State *state, animated_ptr component) {
    state->can_add_new = false;

    while (component->opacity < 1.0) {
        component->opacity += 0.02;
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
    }

    while (component->opacity > 0.) {
        if (state->reset_opacity) {
            component->opacity = 1;
            state->reset_opacity = false;
        }
        component->opacity -= 0.02;
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
    }

    state->can_add_new = true;
}
