#include "opengl/context/context.hpp"
#include <glad.h>
#include <GLFW/glfw3.h>

Context::OpenGL::OpenGL() : GLFW() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void Context::OpenGL::shader_dependent_initialization(std::shared_ptr<TextureShader> shader) {
    // position attribute
    // shader location, size (vec2, vec3 ...), type, normalize,
    // size of 1 stride (line of vertices array), start pos
    GLint mpos{};
    mpos = glGetAttribLocation(shader->ID, "aPos");
    glVertexAttribPointer(mpos, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(mpos);
    // texture coord attribute
    mpos = glGetAttribLocation(shader->ID, "aTexCoord");
    glVertexAttribPointer(
        mpos,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void *)(2 * sizeof(float))
    );
    glEnableVertexAttribArray(mpos);
}

void Context::OpenGL::draw() {
    // Aspect ratio
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Context::OpenGL::swap_and_pull() {
    glfwSwapBuffers(window);
    // if omit this line OS will think program is not responding and will give
    // the "Force quit" / "wait" window
    glfwPollEvents();
}
