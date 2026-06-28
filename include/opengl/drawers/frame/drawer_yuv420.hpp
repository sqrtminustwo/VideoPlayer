#ifndef YUV420_DRAWER_H
#define YUV420_DRAWER_H

#include "opengl/drawers/frame/drawer.hpp"
#include <memory>

struct DrawerYUV420 : Frame::Drawer {

#ifdef __EMSCRIPTEN__
    DrawerYUV420(std::shared_ptr<Context::OpenGL> &context)
        : Drawer(
              context, std::make_shared<TextureShader>(
                           "shaders/image.vs.es.glsl", "shaders/image_yuv420.fs.es.glsl"
                       )
          ){};
#else
    DrawerYUV420(std::shared_ptr<Context::OpenGL> &context)
        : Drawer(
              context, std::make_shared<TextureShader>(
                           "shaders/image.vs.glsl", "shaders/image_yuv420.fs.glsl"
                       )
          ){};
#endif

    int conditional_on_channel(int dim, TextureChannel c) override { return c > 0 ? dim / 2 : dim; }
};

#endif
