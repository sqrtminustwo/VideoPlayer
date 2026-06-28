#ifndef OPENGL_CONTEXT_H
#define OPENGL_CONTEXT_H

#include "opengl/shaders/texture_shader.hpp"
#include <cstdio>
#include <string>

class GLFWwindow;

namespace Context {

class GLFW {

    static void glfw_error_callback(int error, const char *description) {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }

  public:
    GLFWwindow *window;
    float main_scale;

    GLFW();
    ~GLFW();
};

class OpenGL : public GLFW {

    unsigned int VBO, VAO, EBO;
    // std::shared_ptr<TextureShader> shader;

    // clang-format off
    constexpr static float vertices[] = {
        // positions    // texture coords
        1.0f,  1.0f,    1.0,  0.0f, // bottom right
        1.0f,  -1.0f,   1.0f, 1.0f, // top right
        -1.0f, -1.0f,   0.0f, 1.0f, // top left
        -1.0f, 1.0f,    0.0f, 0.0f  // bottom left
    };
    // clang-format on

    constexpr static unsigned int indices[] = {0, 1, 3, 1, 2, 3};

  protected:
#ifdef __EMSCRIPTEN__
    inline static const std::string glsl_version = "#version 300 es";
#else
    inline static const std::string glsl_version = "#version 330";
#endif

  public:
    OpenGL();

    void shader_dependent_initialization(std::shared_ptr<TextureShader>);
    void draw();
    void swap_and_pull();
};

struct MyImGui : public OpenGL {

    MyImGui();
    ~MyImGui();
};

} // namespace Context

#endif
