#include <thread>
#include "opengl/context/context.hpp" // IWYU pragma: keep
#include "ffmpeg/video_player.hpp"    // IWYU pragma: keep
#include "opengl/keyhandler/keyhandler.hpp"
#include "opengl/drawers/overlay/components/animated/animated.hpp"

using namespace std;

KeyHandler::KeyHandler(player_ptr player, components_container &components)
    : player{player}, components{components} {}

animated_ptr KeyHandler::index_to_animated(ComponentsIndex i) {
    return dynamic_pointer_cast<Overlay::Animated>(components[i]);
}

GLFWkeyfun KeyHandler::make_key_callback(opengl_context opengl_context) {
    glfwSetWindowUserPointer(opengl_context->window, this);

    return [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        KeyHandler *keyhandler = (KeyHandler *)glfwGetWindowUserPointer(window);
        auto player = keyhandler->player;
        auto p = (action == GLFW_PRESS);
        auto r_or_p = (action == GLFW_REPEAT) || p;

        if (key == GLFW_KEY_SPACE && p) {
            keyhandler->make_aimation_thread(
                keyhandler->pause,
                keyhandler->index_to_animated(PAUSE)
            );
            player->pause.toggle();
        }
        if (key == GLFW_KEY_LEFT && r_or_p) {
            keyhandler->make_aimation_thread(
                keyhandler->backward,
                keyhandler->index_to_animated(BACKWARD)
            );
            player->skip_seconds_forward(false);
        }
        if (key == GLFW_KEY_RIGHT && r_or_p) {
            keyhandler->make_aimation_thread(
                keyhandler->forward,
                keyhandler->index_to_animated(FORWARD)
            );
            player->skip_seconds_forward(true);
        }
    };
}

void KeyHandler::make_aimation_thread(State &state, animated_ptr component) {
    if (state.can_add_new) {
        thread t(&KeyHandler::animate, this, &state, component);
        t.detach();
    } else component->opacity = 1.f;
}

void sleep_millis(int millis) { this_thread::sleep_for(chrono::milliseconds(millis)); }

void KeyHandler::animate(State *state, animated_ptr component) {
    state->can_add_new = false;
    unsigned int sleep_time = 8;
    float degrade_speed = 0.02;

    while (component->opacity < 1.0) {
        component->opacity += degrade_speed;
        sleep_millis(sleep_time);
    }

    while (component->opacity > 0.) {
        component->opacity -= degrade_speed;
        sleep_millis(sleep_time);
    }

    state->can_add_new = true;
}
