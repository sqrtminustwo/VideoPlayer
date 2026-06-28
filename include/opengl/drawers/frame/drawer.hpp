#ifndef FRAME_DRAWER_H
#define FRAME_DRAWER_H

#include "opengl/context/context.hpp"
#include "opengl/shaders/texture_shader.hpp"
#include "types/constants.hpp"
#include <memory>

namespace Frame {

struct Drawer {

    Drawer(std::shared_ptr<Context::OpenGL>, std::shared_ptr<TextureShader>);

    /** @brief Draw given frame
     *  @param AVFrame to render.
     *  @return void.
     */
    void operator()(const frame_ptr &, const bool &paused);

  protected:
    std::shared_ptr<TextureShader> shader;
    std::shared_ptr<Context::OpenGL> context;

    virtual int conditional_on_channel(int dim, TextureChannel) = 0;
};

} // namespace Frame

#endif
