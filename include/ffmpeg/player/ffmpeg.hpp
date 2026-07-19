#ifndef FFMPEG_META_H
#define FFMPEG_META_H

#include "buffer/buffer.hpp"
#include "ffmpeg/fetcher/file_fetcher.hpp"
#include "ffmpeg/fetcher/js_fetcher.hpp"
#include "types/types.hpp"
#include "ffmpeg/player/stream/stream.hpp" // IWYU pragma: keep

enum LoadStatus { LOADED_AUDIO, LOADED_VIDEO, NO_MORE_FRAMES, ERROR };

struct PacketGuard {
    packet_ptr &packet;

    PacketGuard() = delete;
    PacketGuard(packet_ptr &packet);
    ~PacketGuard();
};

struct FFmpeg {
#ifdef __EMSCRIPTEN__
    JSFetcher fetcher{};
#else
    FileFetcher fetcher{};
#endif
    Buffer *bd;
    static constexpr int avio_ctx_buffer_size = 2097152;

    format_ptr fmt_ctx = make_format_ptr();
    packet_ptr packet = make_packet_ptr();
    avio_ptr avio_ctx = make_avio_ptr();

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

    bool loading_cond(const LoadStatus &status) const;
    LoadStatus load_more_frames();

    double front_frame_timestamp_in_seconds();

    int seek_ts(int64_t &);
    void skip_frames();

    ~FFmpeg();

  private:
    double time_base = 0;

    format_ptr make_format_ptr();
    packet_ptr make_packet_ptr();
    avio_ptr make_avio_ptr();
};

#endif
