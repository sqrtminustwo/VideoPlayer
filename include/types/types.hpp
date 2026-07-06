#ifndef TYPES_H
#define TYPES_H

#include <array>
#include <memory>
#include <chrono>

// include minimization
class AVFrame;
class AVFormatContext;
class AVCodecContext;
class AVPacket;
class AVIOContext;
class AVCodec;
namespace Overlay {
class Component;
class Animated;
class AnimatedComponentsHandle;
} // namespace Overlay
namespace Context {
class OpenGL;
}
class VideoPlayer;

using textures_t = std::array<unsigned int *, 3>;

using opengl_context = std::shared_ptr<Context::OpenGL>;
using player_ptr = std::shared_ptr<VideoPlayer>;
using frame_ptr = std::shared_ptr<AVFrame>;
using format_ptr = std::unique_ptr<AVFormatContext, void (*)(AVFormatContext *)>;
using decoder_ptr = std::unique_ptr<AVCodecContext, void (*)(AVCodecContext *)>;
using packet_ptr = std::unique_ptr<AVPacket, void (*)(AVPacket *)>;
using avio_ptr = std::unique_ptr<AVIOContext, void (*)(AVIOContext *)>;

enum ComponentsIndex { CONTROLLER = 0, PAUSE, BACKWARD, FORWARD, SPINNER };
using components_container = std::array<std::shared_ptr<Overlay::Component>, 5>;
using animated_ptr = std::shared_ptr<Overlay::Animated>;
using ach = Overlay::AnimatedComponentsHandle &;

using time_point = std::chrono::system_clock::time_point;
using duration = std::chrono::duration<float>;

struct AspectRatio {
    int numer;
    int denom;

    AspectRatio(int number, int denom);
};

#endif
