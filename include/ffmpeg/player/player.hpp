#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include <atomic>
#include <mutex>
#include <thread>

#include "ffmpeg/player/last_frame.hpp"
#include "ffmpeg/player/pause.hpp"
#include "ffmpeg/player/ffmpeg.hpp"
#include "types/types.hpp"
#include "ffmpeg/fetcher/js_fetcher.hpp"

enum VideoPlayerState {
    VIDEO_NOT_SET,
    VIDEO_SET_NOT_PLAYED,
    VIDEO_PLAYING,
    SETTING_PLAYED_DURATION
};

class Player {
    FFmpeg ffmpeg;

    std::thread duration_setting_thread;
    void join_duration_setter();

    // Keep last frame for pause / video end
    // so that video is never actually done
    // until it is closed (or an error occured)
    LastFrame last_frame;
    std::atomic<VideoPlayerState> state{VIDEO_NOT_SET};
    time_point start_time;

    auto cast_to_start_time(::duration);

  public:
    std::mutex played_duration_mutex;
    duration played_duration;
    Pause pause;
    int skip_seconds = 5;

    AspectRatio get_aspect_ratio() const;
    duration get_total_duration() const;
    std::string get_total_duration_str() const;

#ifdef __EMSCRIPTEN__
    int set_video();
#else
    int set_video(const std::string &filename);
#endif

    bool is_loading();

    void set_played_duration(const duration &);
    void skip_seconds_forward(bool);

    /** @brief Get next frame of loaded video
     *  @params bool paused, whether video is paused or no
     *  @throws if no video loaded
     *  @return AVFrame* if video ended null_ptr
     */
    LastFrame &operator()();

    ~Player();
};

#endif
