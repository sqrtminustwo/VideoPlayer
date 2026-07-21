#ifndef FRAME_DRAWER_H
#define FRAME_DRAWER_H

#include "ffmpeg/player/last_frame.hpp"
#include "opengl/context/context.hpp"
#include "opengl/shaders/texture_shader.hpp"
#include "types/constants.hpp"
#include <memory>

namespace Frame {

struct Drawer {
    Drawer(std::shared_ptr<Context::OpenGL>, std::shared_ptr<TextureShader>);

    /** @brief Draw given frame
     *  @param LastFrame to render.
     *  @return void.
     */
    void operator()(LastFrame &);

  protected:
    std::shared_ptr<TextureShader> shader;
    std::shared_ptr<Context::OpenGL> context;

    virtual int conditional_on_channel(int &dim, TextureChannel &) = 0;
};

} // namespace Frame

#endif
