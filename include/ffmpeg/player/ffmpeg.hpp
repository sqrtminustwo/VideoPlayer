#ifndef FFMPEG_META_H
#define FFMPEG_META_H

#include "buffer/buffer.hpp"
#include "ffmpeg/fetcher/file_fetcher.hpp"
#include "ffmpeg/fetcher/js_fetcher.hpp"
#include "types/types.hpp"
#include "ffmpeg/player/stream/stream.hpp" // IWYU pragma: keep

enum LoadStatus { LOADED_AUDIO = 0, LOADED_VIDEO, NEED_MORE_PACKETS, ERROR };
#define INITIAL_LOAD_STATUS auto status = NEED_MORE_PACKETS;

struct PacketGuard {
    packet_ptr &packet;

    PacketGuard() = delete;
    PacketGuard(packet_ptr &packet);
    ~PacketGuard();
};

class FFmpeg {
#ifdef __EMSCRIPTEN__
    JSFetcher fetcher{};
#else
    FileFetcher fetcher{};
#endif
    Buffer *bd;

    packet_ptr packet = make_packet_ptr();
    avio_ptr avio_ctx = make_avio_ptr();

    static constexpr int avio_ctx_buffer_size = 2097152;
    double time_base = 0;

    LoadStatus send_packet();
    format_ptr make_format_ptr();
    packet_ptr make_packet_ptr();
    avio_ptr make_avio_ptr();

  public:
    format_ptr fmt_ctx = make_format_ptr();

    stream_ptr video;
    stream_ptr audio;

    AspectRatio aspect_ratio{16, 9};
    std::string total_duration_str;
    duration total_duration;

#ifdef __EMSCRIPTEN__
    int set_video();
#else
    int set_video(const std::string &filename);
#endif

    bool is_loaded(const LoadStatus &status) const;
    LoadStatus load_more_frames();
    LoadStatus skip_frames(LoadStatus skip_until = LOADED_VIDEO);

    double front_frame_timestamp_in_seconds() const;

    ~FFmpeg();
};

#endif
