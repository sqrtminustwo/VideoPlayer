#include "ffmpeg/player/ffmpeg.hpp"
#include "ffmpeg/platform/make_fetcher.hpp"
#include "ffmpeg/player/stream/audio.hpp"
#include "types/frame/frame_ptr.hpp"
#include "ffmpeg/player/stream/video.hpp"
#include "types/types.hpp"
#include "utils/guards/atomic_boolean_guard.hpp"
#include "utils/guards/packet_guard.hpp"
#include "utils/utils.hpp"
#include <atomic>
#include <libavutil/error.h>
#include <memory>

#if defined(DEBUG) || defined(__EMSCRIPTEN__)
#include "buffer/seq_guard.hpp"
#endif

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/file.h>
}

using namespace std;

FFmpeg::~FFmpeg() {
    // Should happend before stopping to wait
    // otherwise infinite wait in loader_thread_loop
    should_load.store(false, memory_order_release);
    // https://isocpp.org/wiki/faq/dtors
    // Because both streams are constructed in a later function call
    // they will be destructed after this destructor, thats why this
    // can't happend in destructor of stream
    for (auto &stream : {audio, video})
        if (stream.get()) stream->frames_queue.stop_waiting();
    join_if_joinable(loader_thread);
}

format_ptr FFmpeg::make_format_ptr() {
    return format_ptr(avformat_alloc_context(), [](AVFormatContext *f) {
        if (!f) return;
        avformat_close_input(&f);
    });
}

avio_ptr FFmpeg::make_avio_ptr() {
    return avio_ptr(nullptr, [](AVIOContext *avio_ctx) {
        if (!avio_ctx) return;
        av_freep(&avio_ctx->buffer);
        av_freep(&avio_ctx);
    });
}

// Dont check for f not being nullptr for efficiency
// as packets are constantly created and freed
packet_ptr FFmpeg::make_packet_ptr() {
    return packet_ptr(av_packet_alloc(), [](AVPacket *f) { av_packet_free(&f); });
}

static int64_t seek(void *opaque, int64_t offset, int whence) {
    auto *bd = (Buffer *)opaque;

    if (whence == AVSEEK_SIZE) return bd->get_total_size();

    // can be ignored
    // https://www.ffmpeg.org/doxygen/2.3/avio_8h.html#afc6af68de5304c6cea23a785c1f94cd5
    whence &= ~AVSEEK_FORCE;

    switch (whence) {
        CASE(SEEK_SET, [&] { bd->set_offset(offset); });
        CASE(SEEK_CUR, [&] { bd->set_offset(bd->get_offset() + offset); });
        CASE(SEEK_END, [&] { bd->set_offset(bd->get_total_size() + offset); });
    }

    if (bd->get_offset() > bd->get_total_size() || bd->get_offset() < 0) return -1;

    return bd->get_offset();
}

// https://www.ffmpeg.org/doxygen/2.3/avio_reading_8c-example.html#_a10
// https://www.ffmpeg.org/doxygen/2.3/aviobuf_8c_source.html#l00200

int FFmpeg::set_video(const string &filename) {
    int ret;

    fetcher = make_fetcher(filename, avio_ctx_buffer_size);
#if defined(DEBUG) || defined(__EMSCRIPTEN__)
    /*
     * On call to avformat_open_input ffmpeg will make
     * 2 small loads in start and end of file
     * to determine type etc..., preloading is not
     * needed for these 2 loads, below guard sets
     * load mode to SEQ until destruction
     */
    SeqGuard guard{dynamic_cast<CyclicFragmentBuffer2 *>(fetcher->bd.get()), FULL};
#endif

    AVIOContext *avio_ctx = NULL;
    uint8_t *avio_ctx_buffer = NULL;

    avio_ctx_buffer = (uint8_t *)av_malloc(avio_ctx_buffer_size);
    if (!avio_ctx_buffer) {
        printf("Failed to alloc / cast avio_ctx_buffer!\n");
        return -1;
    }

    avio_ctx = avio_alloc_context(
        avio_ctx_buffer,
        avio_ctx_buffer_size,
        0,
        fetcher->bd.get(),
        &Buffer::avio_read_packet,
        NULL,
        &seek
    );
    if (!avio_ctx) {
        printf("Failed to alloc avio_ctx!\n");
        return -1;
    }
    this->avio_ctx.reset(avio_ctx);

    fmt_ctx->pb = avio_ctx;
    // already allocated in member variable
    auto format_ptr = fmt_ctx.get();

    if ((ret = avformat_open_input(&format_ptr, NULL, NULL, NULL)) < 0) {
        printf("Could not open input\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(format_ptr, NULL)) < 0) {
        printf("Cannot find stream information\n");
        return ret;
    }

    video = make_unique<Video>(fmt_ctx);
    audio = make_unique<Audio>(fmt_ctx);

    video->init_stream(AVMEDIA_TYPE_VIDEO);
    audio->init_stream(AVMEDIA_TYPE_AUDIO);

    stream_ptr time_base_stream = nullptr;
    if (video->is_valid()) {
        time_base_stream = video;
        aspect_ratio = Resolution(video->dec_ctx->width, video->dec_ctx->height);
    } else if (audio->is_valid()) {
        time_base_stream = audio;
    } else {
        printf("No audio or video stream found, exiting...\n");
        return -1;
    }

    time_base = av_q2d(time_base_stream->get_stream()->time_base);
    total_duration = chrono::duration<float>(fmt_ctx->duration / AV_TIME_BASE);
    total_duration_str = duration_to_string(total_duration);

    should_load.store(true);
    loader_thread = thread(&FFmpeg::loader_thread_loop, this);

    return 0;
}

double FFmpeg::front_frame_timestamp_in_seconds(stream_ptr &stream) const {
    auto front_frame = stream->frames_queue.front_ptr();
    if ((*front_frame).get() == nullptr) return -1;
    return ((double)(*front_frame).get()->pts) * time_base;
}

LoadStatus FFmpeg::get_load_status() const { return load_status.load(memory_order_acquire); }
bool FFmpeg::is_loaded(const LoadStatus &status) const {
    return status == LOADED_VIDEO || status == LOADED_AUDIO;
}

LoadStatus FFmpeg::skip_frames(LoadStatus skip_until) {
    video->frames_queue.clear();

    LoadStatus state;
    do { state = load_more_frames(); } while (state != ERROR && state != skip_until);
    return state;
}

LoadStatus FFmpeg::load_more_frames() {
    LoadStatus status;
    do { status = send_packet(); } while (status == NEED_MORE_PACKETS);
    return status;
}

#define RETURN                                                                                     \
    {                                                                                              \
        auto new_load_status = ret == AVERROR_EOF ? END : ERROR;                                   \
        return (load_status = new_load_status);                                                    \
    }

LoadStatus FFmpeg::send_packet() {
    int ret = 0;

    if ((ret = av_read_frame(fmt_ctx.get(), packet.get())) < 0) RETURN;

    PacketGuard packet_guard{packet};

    load_status = NEED_MORE_PACKETS;
    LoadStatus on_load;
    stream_ptr stream = nullptr;

    if (packet->stream_index == video->stream_index) on_load = LOADED_VIDEO, stream = video;
    else if (packet->stream_index == audio->stream_index) on_load = LOADED_AUDIO, stream = audio;

    if (!stream) return load_status;

    auto dec_ctx_ptr = stream->dec_ctx.get();

    ret = avcodec_send_packet(dec_ctx_ptr, packet.get());

    if (ret < 0) {
        printf("Error while sending a packet to the decoder\n");
        RETURN;
    }

    while (ret >= 0) {
        /*
         * Allocates new frame each time
         */
        frame_ptr frame_ptr{};
        ret = avcodec_receive_frame(dec_ctx_ptr, frame_ptr.get());

        if (ret == AVERROR(EAGAIN)) break;
        else if (ret < 0) RETURN;

        stream->add_frame(std::move(frame_ptr));
        load_status = on_load;
    }

    return load_status;
}

AtomicBooleanGuard FFmpeg::get_should_pause_guard() {
    auto guard = AtomicBooleanGuard{pause_loader.should_pause};
    video->frames_queue.stop_waiting();
    return guard;
}
void FFmpeg::wait_until_loader_thread_paused() {
    if (!pause_loader.should_pause.load(memory_order_acquire)) {
        fprintf(stderr, "Can't wait for loader pause, if should_pause is not set!\n");
        return;
    }

    auto paused = pause_loader.paused.load(memory_order_acquire);
    if (paused) return;
    pause_loader.paused.wait(paused, memory_order_relaxed);
}
void FFmpeg::loader_thread_loop() {
    LoadStatus status;
    bool should_stop = false;

    auto load_stream = [&status, this](stream_ptr stream) {
        status = NEED_MORE_PACKETS;
        while (stream->frames_queue.size() < stream->frames_queue_size_bound &&
               (status != END && status != ERROR))
            status = send_packet();
    };

    while (should_load.load(memory_order_acquire)) {
        should_stop = pause_loader.should_pause.load(memory_order_acquire);
        if (should_stop) {
            AtomicBooleanGuard guard{pause_loader.paused};
            pause_loader.paused.notify_one();
            pause_loader.should_pause.wait(should_stop, memory_order_relaxed);
        }

        load_stream(video);
        // Causes double loads on no audio
        // not good like this alone
        // load_stream(audio);

        video->frames_queue.wait_for_size_change();
    }
}
