#include <array>

// #include "miniaudio/audio_device.hpp"
#include "ffmpeg/player/player.hpp"
#include "fonts/fonts.hpp"
#include "miniaudio/audio_device.hpp"
#include "opengl/drawers/overlay/components/animated/backward.hpp"
#include "opengl/drawers/overlay/components/animated/forward.hpp"
#include "opengl/drawers/overlay/components/animated/pause.hpp"
#include "opengl/drawers/overlay/components/animated/spinner.hpp"
#include "opengl/drawers/overlay/components/controller.hpp"
#include "opengl/drawers/overlay/drawer.hpp"
#include "opengl/drawers/frame/drawer_yuv420.hpp"
#include "opengl/keyhandler/keyhandler.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include "imgui_impl_glfw.h"
#include "emscripten/emscripten_mainloop_stub.h"
#endif

using namespace std;

int main(int argc, char **argv) {
    auto player = make_shared<Player>();

    int ret;
#ifdef __EMSCRIPTEN__
    ret = player->set_video();
#else
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <video>\n", argv[0]);
        exit(0);
    }
    auto filename = argv[1];

    ret = player->set_video(filename);
#endif

    if (ret < 0) {
        fprintf(stderr, "Error setting video!\n");
        exit(1);
    }

    AudioDevice audio_device{player};

    auto myimgui_context = make_shared<Context::MyImGui>();
    auto opengl_context = static_pointer_cast<Context::OpenGL>(myimgui_context);

    auto res = player->get_resolution();
    glfwSetWindowAspectRatio(opengl_context->window, res.width, res.height);
    init_imgui_fonts(opengl_context->main_scale);

    components_container components;
    components[CONTROLLER] = make_shared<Overlay::Controller>(player);
    components[PAUSE] = make_shared<Overlay::Pause>(player);
    components[SPINNER] = make_shared<Overlay::Spinner>(player);
    components[BACKWARD] = make_shared<Overlay::Backward>();
    components[FORWARD] = make_shared<Overlay::Forward>();

    DrawerYUV420 frame_drawer{opengl_context};
    frame_drawer.set_image_resolution(res);
    Overlay::Drawer overlay_drawer{};

    KeyHandler keyhandler{player, components};

    glfwSetKeyCallback(opengl_context->window, keyhandler.make_key_callback(opengl_context));

#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(myimgui_context->window))
#endif
    {
        auto &frame = (*player)();
        if (!frame.get()) break;

        frame_drawer(frame);
        opengl_context->draw();

        overlay_drawer(components);

        // WARNING: there is no need to clear screen
        // new frame will just overvrite old frame + imgui overlay
        // and then new overlay will be drawn on top

        opengl_context->swap_and_pull();
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    return 0;
}
