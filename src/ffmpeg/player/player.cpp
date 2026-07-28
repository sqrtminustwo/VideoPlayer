#include "ffmpeg/player/player.hpp"
#include "ffmpeg/player/ffmpeg.hpp"
#include "ffmpeg/player/stream/video.hpp"
#include "types/types.hpp"
#include "utils/utils.hpp"
#include <libavutil/avutil.h>
#include <memory>
#include "types/constants.hpp"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}

using namespace std;
using namespace chrono_literals;

#define LOCK_PLAYED_DURATION lock_guard<mutex> lock{played_duration_mutex};

Player::~Player() { join_if_joinable(duration_setting_thread); }

Resolution Player::get_resolution() const { return ffmpeg.aspect_ratio; }
duration Player::get_total_duration() const { return ffmpeg.total_duration; }
string Player::get_total_duration_str() const { return ffmpeg.total_duration_str; }

bool Player::is_loading() const { return state == LOADING; }

stream_ptr Player::get_audio_stream() const { return ffmpeg.audio; }
stream_ptr Player::get_video_stream() const { return ffmpeg.video; };

#ifdef __EMSCRIPTEN__
int Player::set_video()
#else
int Player::set_video(const string &filename)
#endif
{
    join_if_joinable(duration_setting_thread);

    int ret = ffmpeg.set_video(
#ifndef __EMSCRIPTEN__
        filename
#endif
    );

    state = VIDEO_SET_NOT_PLAYED;

    last_frame.set(
        std::static_pointer_cast<Video>(ffmpeg.video)->make_black_frame_ptr(ffmpeg.aspect_ratio)
    );

    return ret;
}

LastFrame &Player::operator()() {
    // no locks / mutexes needed below as state is atomic
    // // and no action will be done until skipping thread is working
    if (is_loading()) return last_frame;

    if (state == VIDEO_NOT_SET) {
        fprintf(stderr, "No video set, can't play!");
        return last_frame;
    }

    // There is always a last frame is video is set
    // either black or previous frame
    if (pause.paused_now()) return last_frame;
    if (aprox_played_duration(ffmpeg.total_duration)) {
        played_duration = ffmpeg.total_duration;
        return last_frame;
    }

    if (ffmpeg.get_load_status() == ERROR) {
        played_duration = ffmpeg.total_duration;
        return last_frame;
    }

    auto now = now_f();

    if (state == VIDEO_SET_NOT_PLAYED) {
        state = VIDEO_PLAYING;
        start_time = now;
    }

    if (pause.adjust_player()) {
        start_time += now - pause.pause_time();
        pause.toggle_adjust_player();
    }

    played_duration = duration_diff(now, start_time);

    if (!ffmpeg.video->frames_queue.empty()) {
        auto current = ffmpeg.front_frame_timestamp_in_seconds();
        auto expected = played_duration.count();
        if (current <= expected) {
            this->last_frame.set(ffmpeg.video->frames_queue.front());
            ffmpeg.video->frames_queue.pop_front();
        }
    }

    return last_frame;
}

void Player::skip_seconds_forward(bool forward) {
    auto skip_duration = chrono::seconds(skip_seconds);

    duration new_duration;
    {
        LOCK_PLAYED_DURATION;
        new_duration = forward ? min(played_duration + skip_duration, ffmpeg.total_duration)
                               : max(played_duration - skip_duration, ZERO_TS);
    }

    set_played_duration(new_duration);
}

auto Player::cast_to_start_time(::duration d) const {
    return chrono::duration_cast<typename decltype(this->start_time)::duration>(d);
}
bool Player::aprox_played_duration(::duration d) const {
    return std::chrono::abs(d - played_duration) <= 50ms;
}

void Player::set_played_duration(const duration &new_played_duration) {
    // Already loading, duration below 0, exceeds video length -> don't do anything
    if (is_loading() || (new_played_duration < ZERO_TS) ||
        (new_played_duration > ffmpeg.total_duration) || aprox_played_duration(new_played_duration))
        return;

    const time_point started_setting = now_f();

    // old frames are now invalid
    ffmpeg.video->frames_queue.clear();

    // started_setting = now
    this->start_time = started_setting - cast_to_start_time(new_played_duration);

    join_if_joinable(duration_setting_thread);
    state = LOADING;

    duration_setting_thread = thread([this, new_played_duration, started_setting]() {
        auto should_load_guard = ffmpeg.get_should_pause_guard();
        ffmpeg.wait_until_loader_thread_paused();

        double duration_count = chrono::duration<double>(new_played_duration).count();

        // Seek to keyframe <= the duration
        auto seeked = duration_count;
        double decriment = 1.;
        auto stop = 1e-5;

        ffmpeg.video->frames_queue.clear();
        while (true) {
            if (ffmpeg.video->seek_ts(seeked) < 0) return;
            if (ffmpeg.video->frames_queue.empty()) ffmpeg.skip_frames();

            if (ffmpeg.front_frame_timestamp_in_seconds() <= duration_count) break;

            // Should be after skipping otherwise
            // you wont be able to set time 0
            if (seeked > -stop && seeked < stop) break;

            seeked = max(0., seeked - decriment);
        }

        // Seek forward to exact frame
        duration old_played_duration;
        {
            LOCK_PLAYED_DURATION;
            old_played_duration = played_duration;
            played_duration = new_played_duration;
        }

        auto status = LOADED_VIDEO;
        while (ffmpeg.is_loaded(status) &&
               ffmpeg.front_frame_timestamp_in_seconds() < duration_count)
            status = ffmpeg.skip_frames();

        if (is_loading()) {
            const auto ended_setting = now_f();
            start_time += cast_to_start_time(ended_setting - started_setting);
            if (pause.paused_now()) pause.refresh_pause_time();
            state = VIDEO_PLAYING;
        }
    });
}
