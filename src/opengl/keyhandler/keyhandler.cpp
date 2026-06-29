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
            if (keyhandler->can_add_new_pause) {
                std::thread t(
                    &KeyHandler::animate,
                    keyhandler,
                    std::make_shared<Overlay::Pause>(player),
                    &keyhandler->can_add_new_pause
                );
                t.detach();
            }
            player->pause.toggle();
        }
        if (key == GLFW_KEY_LEFT && r_or_p) {
            if (keyhandler->can_add_new_backward) {
                std::thread t(
                    &KeyHandler::animate,
                    keyhandler,
                    std::make_shared<Overlay::Backward>(player),
                    &keyhandler->can_add_new_backward
                );
                t.detach();
            }
            player->skip_seconds_forward(false);
        }
        if (key == GLFW_KEY_RIGHT && r_or_p) {
            if (keyhandler->can_add_new_forward) {
                std::thread t(
                    &KeyHandler::animate,
                    keyhandler,
                    std::make_shared<Overlay::Forward>(player),
                    &keyhandler->can_add_new_forward
                );
                t.detach();
            }
            player->skip_seconds_forward(true);
        }
    };
}

void KeyHandler::animate(animated_ptr component, bool *can_add_new) {
    *can_add_new = false;

    componets_mutex.lock();
    components.push_back(component);
    componets_mutex.unlock();

    while (component->opacity < 1.0) {
        component->opacity += 0.02;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    while (component->opacity > 0.) {
        component->opacity -= 0.02;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    componets_mutex.lock();
    auto pos = std::find(components.begin(), components.end(), component);
    if (pos != components.end()) components.erase(pos);
    componets_mutex.unlock();

    *can_add_new = true;
}
