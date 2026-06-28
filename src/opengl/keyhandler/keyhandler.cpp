#include <opengl/keyhandler/keyhandler.hpp>
// needed
#include "opengl/context/context.hpp"
#include "ffmpeg/video_player.hpp"

GLFWkeyfun make_key_callback(std::shared_ptr<Context::OpenGL> opengl_context, player_ptr player) {
    glfwSetWindowUserPointer(opengl_context->window, player.get());
    return [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        VideoPlayer *player = (VideoPlayer *)glfwGetWindowUserPointer(window);
        auto p = action == GLFW_PRESS;
        auto r_or_p = action == GLFW_REPEAT || p;
        if (key == GLFW_KEY_SPACE && p) player->pause.toggle();
        if (key == GLFW_KEY_LEFT && r_or_p) player->skip_seconds_forward(false);
        if (key == GLFW_KEY_RIGHT && r_or_p) player->skip_seconds_forward(true);
    };
}
