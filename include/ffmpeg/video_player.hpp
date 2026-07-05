#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include "ffmpeg/pause.hpp"
#include "types/types.hpp"
#include <deque>
#include <string>
#include <atomic>

enum VideoPlayerState {
    VIDEO_NOT_SET,
    VIDEO_SET_NOT_PLAYED,
    VIDEO_PLAYING,
    SETTING_PLAYED_DURATION
};

struct buffer_data {
#ifndef __EMSCRIPTEN__
    uint8_t *base; // Fixed pointer to the start of the file in memory
    // preprocessor directive saves a whopping 1 byte
#endif
    size_t total_size; // Fixed total size of the file
    size_t offset;     // Current reading position
};

class VideoPlayer {
    format_ptr fmt_ctx = make_format_ptr();
    decoder_ptr dec_ctx = make_decoder_ptr();
    packet_ptr packet = make_packet_ptr();
    avio_ptr avio_ctx = make_avio_ptr();
    int video_stream_index = -1;
    std::deque<frame_ptr> frames_queue;
    struct buffer_data bd = {0};

    // Keep last frame for pause / video end
    // so that video is never actually done
    // until it is closed (or an error occured)
    frame_ptr last_frame;
    std::atomic<VideoPlayerState> state{VIDEO_NOT_SET};
    double time_base = 0;
    time_point start_time;
    auto cast_to_start_time(::duration);

    frame_ptr make_frame_ptr();
    format_ptr make_format_ptr();
    decoder_ptr make_decoder_ptr(const AVCodec * = NULL);
    packet_ptr make_packet_ptr();
    avio_ptr make_avio_ptr();

    bool load_more_frames();

    double front_frame_timestamp_in_seconds();

    int seek_ts(int64_t &);
    void skip_frames();

  public:
    AspectRatio aspect_ratio{16, 9};
    std::string total_duration_str;
    duration total_duration;
    duration played_duration;
    Pause pause;
    int skip_seconds = 5;

    bool is_loading();

#ifdef __EMSCRIPTEN__
    int set_video();
#else
    int set_video(const std::string &filename);
#endif

    void set_played_duration(const duration &);
    void skip_seconds_forward(bool forward);

    /** @brief Get next frame of loaded video
     *  @params bool paused, whether video is paused or no
     *  @throws if no video loaded
     *  @return AVFrame* if video ended null_ptr
     */
    frame_ptr operator()();

    ~VideoPlayer();
};

#endif
