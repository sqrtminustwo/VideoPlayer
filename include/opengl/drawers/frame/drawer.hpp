#ifndef FRAME_DRAWER_H
#define FRAME_DRAWER_H

#include "ffmpeg/player/last_frame.hpp"
#include "opengl/context/context.hpp"
#include "opengl/shaders/texture_shader.hpp"
#include "types/constants.hpp"
#include <functional>
#include <memory>

namespace Frame {

using texutre_sender = std::function<void(Resolution &&, uint8_t *, int)>;

enum TexType { FULL, SUB };

struct Drawer {
    Drawer(std::shared_ptr<Context::OpenGL>, std::shared_ptr<TextureShader>);
    void set_image_resolution(Resolution);

    /** @brief Draw given frame
     *  @param LastFrame to render.
     *  @return void.
     */
    void operator()(LastFrame &);

  protected:
    std::shared_ptr<TextureShader> shader;
    std::shared_ptr<Context::OpenGL> context;

    void send_texture(Resolution &, frame_ptr &, texutre_sender);
    virtual int conditional_on_channel(int &dim, TextureChannel &) = 0;
};

} // namespace Frame

#endif
