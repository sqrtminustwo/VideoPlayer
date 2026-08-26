#ifndef TYPES_H
#define TYPES_H

#include "fetcher/data_fetcher.hpp"
#include <array>
#include <functional>
#include <memory>
#include <chrono>

// include minimization
class AVFrame;
class AVFormatContext;
class AVCodecContext;
class AVPacket;
class AVIOContext;
class AVCodec;
class AVStream;
class SwrContext;
namespace Overlay {
class Component;
class Animated;
class AnimatedComponentsHandle;
} // namespace Overlay
namespace Context {
class OpenGL;
}
class Player;
class Stream;
class frame_ptr;
class DataFetcher;

using textures_t = std::array<unsigned int *, 3>;

using opengl_context = std::shared_ptr<Context::OpenGL>;
using player_ptr = std::shared_ptr<Player>;
using stream_ptr = std::shared_ptr<Stream>;
using format_ptr = std::shared_ptr<AVFormatContext>;
using decoder_ptr = std::unique_ptr<AVCodecContext, void (*)(AVCodecContext *)>;
using packet_ptr = std::unique_ptr<AVPacket, void (*)(AVPacket *)>;
using avio_ptr = std::unique_ptr<AVIOContext, void (*)(AVIOContext *)>;
using swr_ptr = std::unique_ptr<SwrContext, void (*)(SwrContext *)>;

enum ComponentsIndex { CONTROLLER = 0, PAUSE, BACKWARD, FORWARD, SPINNER };
enum StreamsIndex { VIDEO = 0, AUDIO };
enum LoadStatus { LOADED_AUDIO = 0, LOADED_VIDEO, NEED_MORE_PACKETS, ERROR, END };
std::ostream &operator<<(std::ostream &, const LoadStatus);
using stream_f_void = std::function<void(stream_ptr &)>;
using stream_f_bool = std::function<bool(stream_ptr &)>;
using components_container = std::array<std::shared_ptr<Overlay::Component>, 5>;
using animated_ptr = std::shared_ptr<Overlay::Animated>;
using ach = Overlay::AnimatedComponentsHandle &;

using time_point = std::chrono::system_clock::time_point;
using duration = std::chrono::duration<float>;
using fetcher_ptr = std::unique_ptr<DataFetcher>;

struct Resolution {
    int width;
    int height;

    Resolution(int w, int h);
};

#define CASE(value, f)                                                                             \
    case value:                                                                                    \
        f();                                                                                       \
        break;

#endif
