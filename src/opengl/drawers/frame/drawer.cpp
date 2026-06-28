#include "opengl/context/context.hpp"
#include "opengl/drawers/frame/drawer.hpp"
#include <memory>
#include <glad.h>
#include <GLFW/glfw3.h>

extern "C" {
#include <libavformat/avformat.h>
}

Frame::Drawer::Drawer(
    std::shared_ptr<Context::OpenGL> context, std::shared_ptr<TextureShader> shader
)
    : context(context), shader(shader) {
    context->shader_dependent_initialization(shader);
};

void Frame::Drawer::operator()(const frame_ptr &frame, const bool &paused) {
    if (paused) return;
    shader->use();

    auto yLoc = glGetUniformLocation(shader->ID, "texture_y");
    glUniform1i(yLoc, 0);
    auto uLoc = glGetUniformLocation(shader->ID, "texture_u");
    glUniform1i(uLoc, 1);
    auto vLoc = glGetUniformLocation(shader->ID, "texture_v");
    glUniform1i(vLoc, 2);

    for (int i = 0; i < NUM_OF_TEXTURES; i++) {
        TextureChannel channel = static_cast<TextureChannel>(i);

        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, *shader->textures.at(i));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_R8,
            conditional_on_channel(frame->width, channel),
            conditional_on_channel(frame->height, channel),
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            frame->data[i]
        );
    }
}
