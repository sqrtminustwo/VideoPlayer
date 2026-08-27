#ifndef FFMPEG_META_H
#define FFMPEG_META_H

#include "types/constants.hpp"
#include "types/types.hpp"
#include "ffmpeg/player/stream/stream.hpp" // IWYU pragma: keep
#include "utils/guards/atomic_boolean_guard.hpp"

#include <thread>

struct PauseThread {
    std::atomic_bool should_pause = false; // Modified only by thread that requests to stop
    std::atomic_bool paused = false;       // Modified only by thread that is being stopped
};

class FFmpeg {
    fetcher_ptr fetcher = nullptr;

    std::atomic<LoadStatus> load_status = NEED_MORE_PACKETS;
    packet_ptr packet = make_packet_ptr();
    avio_ptr avio_ctx = make_avio_ptr();

    static constexpr int avio_ctx_buffer_size = 2097152;

    PauseThread pause_loader;
    std::atomic_bool should_load = false;
    std::thread loader_thread;
    std::array<stream_ptr, NUM_OF_STREAMS> streams;
    format_ptr fmt_ctx = make_format_ptr();

    LoadStatus send_packet();
    format_ptr make_format_ptr();
    packet_ptr make_packet_ptr();
    avio_ptr make_avio_ptr();
    frame_ptr make_black_frame();

  public:
    stream_ptr &audio();
    stream_ptr &video();
#define streams_oneliner(oneliner) [](stream_ptr stream, FFmpeg *, void *) { oneliner; }
    void execute_on_streams(stream_f_void &&, void * = nullptr);
    bool conditional_on_streams(stream_f_bool &&);

    Resolution aspect_ratio{16, 9};
    duration total_duration;
    std::string total_duration_str;

    int set_video(const std::string &filename);

    AtomicBooleanGuard get_should_pause_guard();
    void wait_until_loader_thread_paused();

    LoadStatus get_load_status() const;
    bool is_loaded(const LoadStatus &status) const;
    LoadStatus skip_frames(stream_ptr &);
    int seek_ts(const double &);

    double front_frame_timestamp_in_seconds(stream_ptr &) const;

    void loader_thread_loop();

    ~FFmpeg();
};

#endif
