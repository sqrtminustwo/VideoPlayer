#ifndef TYPES_H
#define TYPES_H

#include <functional>
#include <memory>

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
class FFmpeg;

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
enum VideoPlayerState { VIDEO_NOT_SET, VIDEO_SET_NOT_PLAYED, VIDEO_PLAYING, LOADING };
enum LoadStatus { LOADED_AUDIO = 0, LOADED_VIDEO, NEED_MORE_PACKETS, ERROR, END };
enum StreamsIndex { VIDEO = 0, AUDIO };
std::ostream &operator<<(std::ostream &, const LoadStatus);

using stream_f_void = std::function<void(stream_ptr &, FFmpeg *, void *)>;
using stream_f_bool = std::function<bool(stream_ptr &)>;
using components_container = std::array<std::shared_ptr<Overlay::Component>, 5>;
using animated_ptr = std::shared_ptr<Overlay::Animated>;
using ach = Overlay::AnimatedComponentsHandle &;

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
