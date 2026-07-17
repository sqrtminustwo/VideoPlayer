#include "ffmpeg/player/video_player.hpp"
#include "utils/utils.hpp"

extern "C" {
#include <libavformat/avformat.h>
}

using namespace std;

AspectRatio VideoPlayer::get_aspect_ratio() const { return ffmpeg.aspect_ratio; }
duration VideoPlayer::get_total_duration() const { return ffmpeg.total_duration; }
std::string VideoPlayer::get_total_duration_str() const { return ffmpeg.total_duration_str; }

bool VideoPlayer::is_loading() { return state == SETTING_PLAYED_DURATION; }

#ifdef __EMSCRIPTEN__
int VideoPlayer::set_video()
#else
int VideoPlayer::set_video(const std::string &filename)
#endif
{
    join_duration_setter();

    int ret;
#ifdef __EMSCRIPTEN__
    ret = ffmpeg.set_video();
#else
    ret = ffmpeg.set_video(filename);
#endif

    state = VIDEO_SET_NOT_PLAYED;

    return ret;
}

LastFrame &VideoPlayer::operator()() {
    // no locks / mutexes needed below as state is atomic
    // and no action will be done until skipping thread is working
    if (is_loading()) return last_frame;

    if (state == VIDEO_NOT_SET) {
        printDebug("No video set, can't play!");
        return last_frame;
    }

    if (pause.paused_now && last_frame.get()) return last_frame;

    if (ffmpeg.frames_queue.empty() && !ffmpeg.load_more_frames()) {
        // File ended
        played_duration = ffmpeg.total_duration;
        return last_frame;
    }

    auto now = now_f();

    if (state == VIDEO_SET_NOT_PLAYED) {
        state = VIDEO_PLAYING;
        start_time = now;
    }

    if (pause.adjust_player) {
        start_time += now - pause.pause_time;
        pause.adjust_player = false;
    }

    played_duration = duration_diff(now, start_time);

    if (!ffmpeg.frames_queue.empty()) {
        auto current = ffmpeg.front_frame_timestamp_in_seconds();
        auto expected = played_duration.count();
        if (current <= expected) {
            this->last_frame.set(ffmpeg.frames_queue.front());
            ffmpeg.frames_queue.pop_front();
        }
    }

    return last_frame;
}

void VideoPlayer::skip_seconds_forward(bool forward) {
    auto duration = chrono::seconds(skip_seconds);

    played_duration_mutex.lock();
    auto new_duration = forward ? played_duration + duration : played_duration - duration;
    played_duration_mutex.unlock();

    set_played_duration(new_duration);
}

auto VideoPlayer::cast_to_start_time(::duration d) {
    return chrono::duration_cast<typename decltype(this->start_time)::duration>(d);
}

void VideoPlayer::join_duration_setter() {
    if (duration_setting_thread.joinable()) duration_setting_thread.join();
}

void VideoPlayer::set_played_duration(const duration &duration) {
    // Duration below 0, exceeds video length -> don't do anything
    if (is_loading() || (duration < chrono::duration<float>(0)) ||
        (duration > ffmpeg.total_duration))
        return;

    const auto started_setting = now_f();

    // old frames are now invalid
    ffmpeg.frames_queue.clear();
    if (duration > played_duration)
        this->start_time -= cast_to_start_time(duration - played_duration);
    else if (duration < played_duration)
        this->start_time += cast_to_start_time(played_duration - duration);

    /*
     * There seem to be no problem with seeking forward in time
     * seeking backwards requires multiple same avformat_seek_file
     * calls, after research seeking file / frame is a long
     * time problematic part of ffmpeg so its not my
     * skill issue
     */

    join_duration_setter();
    state = SETTING_PLAYED_DURATION;
    duration_setting_thread = thread([this, duration, started_setting]() {
        double duration_count = chrono::duration<double>(duration).count();
        int64_t ts = av_rescale_q(duration_count, {1, 1}, ffmpeg.get_video_streams()->time_base);

        // initial
        if (ffmpeg.seek_ts(ts) < 0) return;
        ffmpeg.load_more_frames();

        played_duration_mutex.lock();
        auto old_played_duration = played_duration;
        played_duration = duration;
        played_duration_mutex.unlock();

        if (duration < old_played_duration)
            while (ffmpeg.front_frame_timestamp_in_seconds() > duration_count) ffmpeg.skip_frames();

        // skip to real seeked time
        while (ffmpeg.front_frame_timestamp_in_seconds() < duration_count) ffmpeg.skip_frames();

        if (is_loading()) {
            state = VIDEO_PLAYING;
            const auto ended_setting = now_f();
            start_time += cast_to_start_time(ended_setting - started_setting);
        }
    });
}

VideoPlayer::~VideoPlayer() { join_duration_setter(); }
