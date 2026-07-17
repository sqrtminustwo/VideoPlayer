#include "ffmpeg/player/video_player.hpp"
#include "fonts/fonts.hpp"
#include "opengl/drawers/overlay/components/animated/backward.hpp"
#include "opengl/drawers/overlay/components/animated/forward.hpp"
#include "opengl/drawers/overlay/components/animated/pause.hpp"
#include "opengl/drawers/overlay/components/animated/spinner.hpp"
#include "opengl/drawers/overlay/components/controller.hpp"
#include "opengl/drawers/overlay/drawer.hpp"
#include "stb_image.h"
#include "opengl/drawers/frame/drawer_yuv420.hpp"
#include "opengl/keyhandler/keyhandler.hpp"
#include <array>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include "imgui_impl_glfw.h"
#include "emscripten/emscripten_mainloop_stub.h"
#endif

int main(int argc, char **argv) {
    auto player = std::make_shared<VideoPlayer>();

#ifdef __EMSCRIPTEN__
    auto ret = player->set_video();
#else
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <video>\n", argv[0]);
        exit(0);
    }
    auto filename = argv[1];

    auto ret = player->set_video(filename);
#endif

    if (ret < 0) {
        printf("Error setting video!\n");
        exit(1);
    }

    auto myimgui_context = std::make_shared<Context::MyImGui>();
    auto opengl_context = std::static_pointer_cast<Context::OpenGL>(myimgui_context);
    glfwSetWindowAspectRatio(
        opengl_context->window,
        player->get_aspect_ratio().numer,
        player->get_aspect_ratio().denom
    );

    init_imgui_fonts(opengl_context->main_scale);

    components_container components;
    components[CONTROLLER] = std::make_shared<Overlay::Controller>(player);
    components[PAUSE] = std::make_shared<Overlay::Pause>(player);
    components[SPINNER] = std::make_shared<Overlay::Spinner>(player);
    components[BACKWARD] = std::make_shared<Overlay::Backward>();
    components[FORWARD] = std::make_shared<Overlay::Forward>();

    bool show_demo_window = true;
    bool p_open = true;

    DrawerYUV420 frame_drawer{opengl_context};
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

        // WARNING: for now ther is no need to clear screen
        // new frame will just overvrite old frame + imgui overlay
        // and then new overlay will be drawn on top

        frame_drawer(frame);
        opengl_context->draw();

        overlay_drawer(&p_open, components);

        opengl_context->swap_and_pull();
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    return 0;
}
