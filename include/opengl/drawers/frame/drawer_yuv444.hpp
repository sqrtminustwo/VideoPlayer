#ifndef YUV444_DRAWER_H
#define YUV444_DRAWER_H

#include "opengl/drawers/frame/drawer.hpp"

struct DrawerYUV444 : Frame::Drawer {

    DrawerYUV444(std::shared_ptr<Context::OpenGL> &context)
        : Drawer(
              context, std::make_shared<TextureShader>(
                           "shaders/image.vs.glsl", "shaders/image_yuv444.fs.glsl"
                       )
          ){};

    int conditional_on_channel(int dim, TextureChannel) override { return dim; }
};

#endif
