#include "ffmpeg/player/ffmpeg.hpp"
#include "ffmpeg/player/stream/audio.hpp"
#include "ffmpeg/player/stream/video.hpp"
#include "utils/utils.hpp"

#if defined(DEBUG) || defined(__EMSCRIPTEN__)
#include "buffer/cfb.hpp"
#else
#include "buffer/default_buffer.hpp"
#endif

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/file.h>
}

using namespace std;

format_ptr FFmpeg::make_format_ptr() {
    return format_ptr(avformat_alloc_context(), [](AVFormatContext *f) {
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

packet_ptr FFmpeg::make_packet_ptr() {
    return packet_ptr(av_packet_alloc(), [](AVPacket *f) { av_packet_free(&f); });
}

static int64_t seek(void *opaque, int64_t offset, int whence) {
    auto *bd = (Buffer *)opaque;

    if (whence == AVSEEK_SIZE) return bd->get_total_size();

    // can be ignored
    // https://www.ffmpeg.org/doxygen/2.3/avio_8h.html#afc6af68de5304c6cea23a785c1f94cd5
    whence &= ~AVSEEK_FORCE;

    if (whence == SEEK_SET) bd->set_offset(offset);
    else if (whence == SEEK_CUR) bd->set_offset(bd->get_offset() + offset);
    else if (whence == SEEK_END) bd->set_offset(bd->get_total_size() + offset);

    if (bd->get_offset() > bd->get_total_size() || bd->get_offset() < 0) return -1;

    return bd->get_offset();
}

// https://www.ffmpeg.org/doxygen/2.3/avio_reading_8c-example.html#_a10
// https://www.ffmpeg.org/doxygen/2.3/aviobuf_8c_source.html#l00200

#ifdef __EMSCRIPTEN__
int FFmpeg::set_video()
#else
int FFmpeg::set_video(const string &filename)
#endif
{
    int ret;

#ifndef __EMSCRIPTEN__
    uint8_t *buffer = NULL;
    size_t buffer_size;

    /* slurp file content into buffer */
    ret = av_file_map(filename.c_str(), &buffer, &buffer_size, 0, NULL);
    if (ret < 0) {
        printf("Failed to open file!\n");
        return -1;
    }

#ifdef DEBUG
    fetcher.file = buffer;
    fetcher.file_size = buffer_size;
#endif
#endif

#if defined(DEBUG) || defined(__EMSCRIPTEN__)
    bd = new CyclicFragmentBuffer{&fetcher, avio_ctx_buffer_size * 4};
#else
    bd = new DefaultBuffer();

    bd->set_base(buffer);
    bd->set_total_size(buffer_size);
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
        bd,
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

    bool required;
    video->init_stream(AVMEDIA_TYPE_VIDEO, required = true);
    audio->init_stream(AVMEDIA_TYPE_AUDIO, required = false);

    time_base = av_q2d(video->get_stream()->time_base);
    aspect_ratio = AspectRatio(video->dec_ctx->width, video->dec_ctx->height);
    total_duration = chrono::duration<float>(fmt_ctx->duration / AV_TIME_BASE);
    total_duration_str = duration_to_string(total_duration);

    return 0;
}

PacketGuard::PacketGuard(packet_ptr &packet) : packet{packet} {}
PacketGuard::~PacketGuard() { av_packet_unref(packet.get()); }

double FFmpeg::front_frame_timestamp_in_seconds() {
    if (video->frames_queue.empty()) return 0;
    return ((double)video->frames_queue.front()->pts) * time_base;
}

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

LoadStatus FFmpeg::send_packet() {
    if (av_read_frame(fmt_ctx.get(), packet.get()) < 0) return ERROR;

    // Declaration above
    PacketGuard packet_guard{packet};

    // WARNING: temporary, audio not implemented yet
    // if (packet->stream_index != video->stream_index) return LOADED_AUDIO;

    LoadStatus status = NEED_MORE_PACKETS, on_load;
    stream_ptr stream = nullptr;

    if (packet->stream_index == video->stream_index) on_load = LOADED_VIDEO, stream = video;
    else if (packet->stream_index == audio->stream_index) on_load = LOADED_AUDIO, stream = audio;

    if (!stream) return status;

    int ret = 0;
    auto dec_ctx_ptr = stream->dec_ctx.get();

    unsigned int i = 0;

    ret = avcodec_send_packet(dec_ctx_ptr, packet.get());

    if (ret < 0) {
        printf("Error while sending a packet to the decoder\n");
        return ERROR;
    }

    while (ret >= 0) {
        /*
         * Allocates new frame each time
         */
        auto frame_ptr = make_frame_ptr();
        ret = avcodec_receive_frame(dec_ctx_ptr, frame_ptr.get());

        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
        else if (ret < 0) {
            printf("Error while receiving a frame from the decoder\n");
            return ERROR;
        }

        stream->add_frame(std::move(frame_ptr));
        status = on_load;
    }

    return status;
}

FFmpeg::~FFmpeg() {
#ifndef __EMSCRIPTEN__
    uint8_t *base;
    size_t total_size;

#ifdef DEBUG
    base = fetcher.file;
    total_size = fetcher.file_size;
#else
    base = bd->get_base();
    total_size = bd->get_total_size();
#endif

    av_file_unmap(base, total_size - bd->get_offset());
#endif
    delete bd;
}
