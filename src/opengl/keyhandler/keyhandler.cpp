#include <thread>
#include "opengl/context/context.hpp" // IWYU pragma: keep
#include "ffmpeg/player/player.hpp"   // IWYU pragma: keep
#include "opengl/keyhandler/keyhandler.hpp"
#include "opengl/drawers/overlay/components/animated/animated.hpp"

using namespace std;

KeyHandler::KeyHandler(player_ptr player, components_container &components)
    : player{player}, components{components} {}

animated_ptr KeyHandler::index_to_animated(ComponentsIndex i) {
    return dynamic_pointer_cast<Overlay::Animated>(components[i]);
}

#define HANDLE_KEY(key, execute)                                                                   \
    case key:                                                                                      \
        execute();                                                                                 \
        break;

GLFWkeyfun KeyHandler::make_key_callback(opengl_context opengl_context) {
    glfwSetWindowUserPointer(opengl_context->window, this);

    return [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        KeyHandler *keyhandler = (KeyHandler *)glfwGetWindowUserPointer(window);
        auto player = keyhandler->player;
        auto p = (action == GLFW_PRESS);
        auto r_or_p = (action == GLFW_REPEAT) || p;

        switch (key) {
            HANDLE_KEY(GLFW_KEY_SPACE, [&] {
                if (!p) return;
                keyhandler->make_aimation_thread(
                    keyhandler->pause,
                    keyhandler->index_to_animated(PAUSE)
                );
                player->pause.toggle();
            });
            HANDLE_KEY(GLFW_KEY_LEFT, [&] {
                if (!r_or_p) return;
                keyhandler->make_aimation_thread(
                    keyhandler->backward,
                    keyhandler->index_to_animated(BACKWARD)
                );
                player->skip_seconds_forward(false);
            });
            HANDLE_KEY(GLFW_KEY_RIGHT, [&] {
                if (!r_or_p) return;
                keyhandler->make_aimation_thread(
                    keyhandler->forward,
                    keyhandler->index_to_animated(FORWARD)
                );
                player->skip_seconds_forward(true);
            });
        }
    };
}

void KeyHandler::make_aimation_thread(State &state, animated_ptr component) {
    if (state.can_add_new) {
        thread t(&KeyHandler::animate, this, &state, component);
        t.detach();
    } else component->opacity.store(1.f);
}

void sleep_millis(int millis) { this_thread::sleep_for(chrono::milliseconds(millis)); }

StateModifier::StateModifier(std::atomic_bool &can_add_new) : can_add_new{can_add_new} {
    this->can_add_new = false;
}
StateModifier::~StateModifier() { can_add_new = true; }

void KeyHandler::animate(State *state, animated_ptr component) {
    StateModifier s{state->can_add_new};
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
}
