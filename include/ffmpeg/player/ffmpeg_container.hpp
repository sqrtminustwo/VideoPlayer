#ifndef FFMPEG_META_H
#define FFMPEG_META_H

#include <deque>

#include "buffer/buffer.hpp"
#include "ffmpeg/fetcher/file_fetcher.hpp"
#include "ffmpeg/fetcher/js_fetcher.hpp"
#include "types/types.hpp"

struct FFmpegContainer {
#ifdef __EMSCRIPTEN__
    JSFetcher fetcher{};
#else
    FileFetcher fetcher{};
#endif
    Buffer *bd;
    static constexpr int avio_ctx_buffer_size = 2097152;

    AVStream *get_video_streams() const;
    format_ptr fmt_ctx = make_format_ptr();
    decoder_ptr dec_ctx = make_decoder_ptr();
    packet_ptr packet = make_packet_ptr();
    avio_ptr avio_ctx = make_avio_ptr();
    int video_stream_index = -1;

    AspectRatio aspect_ratio{16, 9};
    std::string total_duration_str;
    duration total_duration;

    std::deque<frame_ptr> frames_queue;

#ifdef __EMSCRIPTEN__
    int set_video();
#else
    int set_video(const std::string &filename);
#endif

    bool load_more_frames();

    double front_frame_timestamp_in_seconds();

    int seek_ts(int64_t &);
    void skip_frames();

    ~FFmpegContainer();

  private:
    double time_base = 0;

    frame_ptr make_frame_ptr();
    format_ptr make_format_ptr();
    decoder_ptr make_decoder_ptr(const AVCodec * = NULL);
    packet_ptr make_packet_ptr();
    avio_ptr make_avio_ptr();
};

#endif
