#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include <atomic>
#include <mutex>
#include <thread>

#include "types/frame/last_frame.hpp"
#include "ffmpeg/player/pause.hpp"
#include "ffmpeg/player/ffmpeg.hpp"
#include "types/types.hpp"

class Player {
    FFmpeg ffmpeg;

    std::mutex played_duration_mutex;
    std::thread duration_setting_thread;

    // Keep last frame for pause / video end
    // so that video is never actually done
    // until it is closed (or an error occured)
    LastFrame last_frame;
    std::atomic<VideoPlayerState> state{VIDEO_NOT_SET};
    time_point start_time;

    auto cast_to_start_time(::duration) const;

    bool aprox_played_duration(::duration) const;

  public:
    duration played_duration;
    Pause pause;
    int skip_seconds = 5;

    Resolution get_resolution() const;
    duration get_total_duration() const;
    std::string get_total_duration_str() const;

    stream_ptr get_audio_stream();
    stream_ptr get_video_stream();

    int set_video(const std::string &filename = nullptr);

    bool is_loading() const;
    bool is_stalled() const;

    void set_played_duration(const duration &);
    void skip_seconds_forward(bool);

    /** @brief Get next frame of loaded video
     *  @params bool paused, whether video is paused or no
     *  @throws if no video loaded
     *  @return AVframe_ptr* if video ended null_ptr
     */
    LastFrame &operator()();

    ~Player();
};

#endif
