#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include "ffmpeg/actions/pause.hpp"
#include "types/types.hpp"
#include <deque>
#include <string>

struct buffer_data {
    uint8_t *base;     // Fixed pointer to the start of the file in memory
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
    bool started_playing_loaded_video = false;
    double time_base = 0;
    time_point start_time;

    frame_ptr make_frame_ptr();
    format_ptr make_format_ptr();
    decoder_ptr make_decoder_ptr(const AVCodec * = NULL);
    packet_ptr make_packet_ptr();
    avio_ptr make_avio_ptr();

    bool load_more_frames();

    double front_frame_timestamp_in_seconds();

  public:
    AspectRatio aspect_ratio{16, 9};
    std::string total_duration_str;
    duration total_duration;
    duration played_duration;
    Pause pause;
    int skip_seconds = 5;

    ~VideoPlayer();

    int set_video(const std::string &filename);

    void set_played_duration(const duration &);
    void skip_seconds_forward(bool forward);

    /** @brief Get next frame of loaded video
     *  @params bool paused, whether video is paused or no
     *  @throws if no video loaded
     *  @return AVFrame* if video ended null_ptr
     */
    frame_ptr operator()();
};

#endif
