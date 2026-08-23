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

void Frame::Drawer::send_texture(Resolution &res, frame_ptr &frame, texutre_sender sender) {
    for (int i = 0; i < NUM_OF_TEXTURES; i++) {
        TextureChannel channel = static_cast<TextureChannel>(i);

        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, *shader->textures.at(i));

        sender(
            Resolution{
                conditional_on_channel(res.width, channel),
                conditional_on_channel(res.height, channel)
            },
            frame ? frame->data[i] : nullptr,
            i
        );
    }
}

void Frame::Drawer::set_image_resolution(Resolution res) {
    frame_ptr frame = nullptr;
    send_texture(res, frame, [](Resolution &&res, uint8_t *_, int i) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_R8,
            res.width,
            res.height,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            NULL
        );
    });
}

void Frame::Drawer::operator()(LastFrame &last_frame) {
    if (!last_frame.should_send_to_gpu) return;

    // This is not needed since we only use 1 shader
    // shader->use();
    auto res = Resolution{last_frame->width, last_frame->height};

    send_texture(res, last_frame, [](Resolution &&res, uint8_t *data, int i) {
        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0,
            0,
            res.width,
            res.height,
            GL_RED,
            GL_UNSIGNED_BYTE,
            data
        );
    });

    last_frame.should_send_to_gpu = false;
}
