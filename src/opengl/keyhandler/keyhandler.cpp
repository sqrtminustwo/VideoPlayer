#include "opengl/drawers/overlay/components/animated/pause.hpp"
#include "opengl/drawers/overlay/components/animated/backward.hpp"
#include "opengl/drawers/overlay/components/animated/forward.hpp"
#include <opengl/keyhandler/keyhandler.hpp>
#include <thread>
#include "opengl/context/context.hpp" // IWYU pragma: keep
#include "ffmpeg/video_player.hpp"    // IWYU pragma: keep

KeyHandler::KeyHandler(
    player_ptr player, components_vector &components, std::mutex &components_mutex
)
    : player{player}, components{components}, componets_mutex{components_mutex} {}

GLFWkeyfun KeyHandler::make_key_callback(opengl_context opengl_context) {
    glfwSetWindowUserPointer(opengl_context->window, this);
    return [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        KeyHandler *keyhandler = (KeyHandler *)glfwGetWindowUserPointer(window);
        auto player = keyhandler->player;
        auto p = action == GLFW_PRESS;
        auto r_or_p = action == GLFW_REPEAT || p;
        if (key == GLFW_KEY_SPACE && p) {
            keyhandler->make_aimation_thread(
                std::make_shared<Overlay::Pause>(player),
                keyhandler->pause
            );
            player->pause.toggle();
        }
        if (key == GLFW_KEY_LEFT && r_or_p) {
            keyhandler->make_aimation_thread(
                std::make_shared<Overlay::Backward>(player),
                keyhandler->backward
            );
            player->skip_seconds_forward(false);
        }
        if (key == GLFW_KEY_RIGHT && r_or_p) {
            keyhandler->make_aimation_thread(
                std::make_shared<Overlay::Forward>(player),
                keyhandler->forward
            );
            player->skip_seconds_forward(true);
        }
    };
}

void KeyHandler::make_aimation_thread(animated_ptr component, State &state) {
    if (state.can_add_new) {
        std::thread t(&KeyHandler::animate, this, component, &state);
        t.detach();
    } else state.reset_opacity = true;
}

void KeyHandler::animate(animated_ptr component, State *state) {
    state->can_add_new = false;

    componets_mutex.lock();
    components.push_back(component);
    componets_mutex.unlock();

    while (component->opacity < 1.0) {
        component->opacity += 0.02;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    while (component->opacity > 0.) {
        if (state->reset_opacity) {
            component->opacity = 1;
            state->reset_opacity = false;
        }
        component->opacity -= 0.02;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    componets_mutex.lock();
    auto pos = std::find(components.begin(), components.end(), component);
    if (pos != components.end()) components.erase(pos);
    componets_mutex.unlock();

    state->can_add_new = true;
}
