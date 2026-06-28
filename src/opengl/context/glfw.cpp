#include "imgui_impl_glfw.h"
#include "opengl/context/context.hpp"
#include <glad.h>
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

Context::GLFW::GLFW() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    constexpr int logical_width = 1280;
    constexpr int logical_height = 720;

#ifdef __EMSCRIPTEN__
    main_scale = static_cast<float>(emscripten_get_device_pixel_ratio());
#else
    main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
#endif
    if (main_scale <= 0.0f) main_scale = 1.0f;

#ifdef __EMSCRIPTEN__
    const int window_width = static_cast<int>(logical_width * main_scale);
    const int window_height = static_cast<int>(logical_height * main_scale);
#else
    const int window_width = logical_width;
    const int window_height = logical_height;
#endif

    window = glfwCreateWindow(window_width, window_height, "WebStream", nullptr, nullptr);
    if (window == nullptr) return;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return;
    }
}

Context::GLFW::~GLFW() {
    printf("called GLFW destructor\n");
    glfwDestroyWindow(window);
    glfwTerminate();
}
