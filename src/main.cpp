#include "ffmpeg/video_player.hpp"
#include "fonts/fonts.hpp"
#include "opengl/drawers/overlay/components/controller.hpp"
#include "opengl/drawers/overlay/drawer.hpp"
#include "stb_image.h"
#include "opengl/drawers/frame/drawer_yuv420.hpp"
#include "emscripten/emscripten_mainloop_stub.h" // IWYU pragma: keep
#include "opengl/keyhandler/keyhandler.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include "imgui_impl_glfw.h"

extern "C" {
void fetchFrames(int, int, uint8_t *);
}
#endif

int main(int argc, char **argv) {
#ifdef __EMSCRIPTEN__
    uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * 2);
    fetchFrames(1, 2, buffer);
    printf("C++ output: %d, %d\n", buffer[0], buffer[1]);
    free(buffer);
#endif

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <video>\n", argv[0]);
        exit(0);
    }
    auto filename = argv[1];

    auto player = std::make_shared<VideoPlayer>();
    auto ret = player->set_video(filename);
    if (ret < 0) {
        printf("Error setting video!\n");
        exit(1);
    }

    auto myimgui_context = std::make_shared<Context::MyImGui>();
    auto opengl_context = std::static_pointer_cast<Context::OpenGL>(myimgui_context);
    glfwSetWindowAspectRatio(
        opengl_context->window,
        player->aspect_ratio.numer,
        player->aspect_ratio.denom
    );

    init_imgui_fonts(opengl_context->main_scale);

    // Will be expanded by animated icons dynamically, thats why vector
    components_vector components = {std::make_shared<Overlay::Controller>(player)};
    std::mutex components_mutex;

    bool show_demo_window = true;
    bool p_open = true;

    DrawerYUV420 frame_drawer{opengl_context};
    Overlay::Drawer overlay_drawer{components_mutex};

    frame_ptr frame;

    KeyHandler keyhandler{player, components, components_mutex};

    glfwSetKeyCallback(opengl_context->window, keyhandler.make_key_callback(opengl_context));

#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(myimgui_context->window))
#endif
    {
        frame = (*player)();
        if (!frame) break;

        // WARNING: for now ther is no need to clear screen
        // new frame will just overvrite old frame + imgui overlay
        // and then new overlay will be drawn on top

        frame_drawer(frame, player->pause.paused_now);
        opengl_context->draw();

        overlay_drawer(&p_open, components);

        opengl_context->swap_and_pull();
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    return 0;
}
