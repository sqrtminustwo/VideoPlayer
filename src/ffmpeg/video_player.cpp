#include "ffmpeg/video_player.hpp"
#include "utils/utils.hpp"
#include <chrono>
#include <string>
#include <thread>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/file.h>
}

// capi.js
#ifdef __EMSCRIPTEN__
extern "C" {
void fetchFrames(int offset, int length, uint8_t *);
int getTotalSize();
}
#endif

using namespace std;

VideoPlayer::~VideoPlayer() {
#ifndef __EMSCRIPTEN__
    av_file_unmap(bd.base, bd.total_size - bd.offset);
#endif
}

frame_ptr VideoPlayer::make_frame_ptr() {
    return frame_ptr(av_frame_alloc(), [](AVFrame *f) { av_frame_free(&f); });
}

format_ptr VideoPlayer::make_format_ptr() {
    return format_ptr(avformat_alloc_context(), [](AVFormatContext *f) {
        avformat_close_input(&f);
    });
}

avio_ptr VideoPlayer::make_avio_ptr() {
    return avio_ptr(nullptr, [](AVIOContext *avio_ctx) {
        if (!avio_ctx) return;
        av_freep(&avio_ctx->buffer);
        av_freep(&avio_ctx);
    });
}

decoder_ptr VideoPlayer::make_decoder_ptr(const AVCodec *dec) {
    // Initial initialization
    if (dec == NULL) return decoder_ptr(nullptr, [](AVCodecContext *) {});

    return decoder_ptr(avcodec_alloc_context3(dec), [](AVCodecContext *f) {
        avcodec_free_context(&f);
    });
}

packet_ptr VideoPlayer::make_packet_ptr() {
    return packet_ptr(av_packet_alloc(), [](AVPacket *f) { av_packet_free(&f); });
}

// https://www.ffmpeg.org/doxygen/2.3/avio_reading_8c-example.html#_a10
// https://www.ffmpeg.org/doxygen/2.3/aviobuf_8c_source.html#l00200

static int read_packet(void *opaque, uint8_t *buf, int buf_size) {
    struct buffer_data *bd = (struct buffer_data *)opaque;

    int bytes_left = bd->total_size - bd->offset;
    buf_size = FFMIN(buf_size, bytes_left);

    if (buf_size <= 0) return AVERROR_EOF; // End of file

#ifdef __EMSCRIPTEN__
    fetchFrames(bd->offset, buf_size, buf);
#else
    memcpy(buf, bd->base + bd->offset, buf_size);
#endif

    bd->offset += buf_size;

    return buf_size;
}

static int64_t seek(void *opaque, int64_t offset, int whence) {
    buffer_data *bd = (buffer_data *)opaque;

    if (whence == AVSEEK_SIZE) return bd->total_size;

    // can be ignored
    // https://www.ffmpeg.org/doxygen/2.3/avio_8h.html#afc6af68de5304c6cea23a785c1f94cd5
    whence &= ~AVSEEK_FORCE;

    if (whence == SEEK_SET) bd->offset = offset;
    else if (whence == SEEK_CUR) bd->offset += offset;
    else if (whence == SEEK_END) bd->offset = bd->total_size + offset;

    if (bd->offset > bd->total_size || bd->offset < 0) return -1;

    return bd->offset;
}

#ifdef __EMSCRIPTEN__
int VideoPlayer::set_video()
#else
int VideoPlayer::set_video(const string &filename)
#endif
{
    const AVCodec *dec;
    int ret;

    AVIOContext *avio_ctx = NULL;
    uint8_t *avio_ctx_buffer = NULL;
    // 1 MiB
    size_t avio_ctx_buffer_size = 1024 * 1024;

#ifdef __EMSCRIPTEN__
    bd.total_size = getTotalSize();
#else
    uint8_t *buffer = NULL;
    size_t buffer_size;

    /* slurp file content into buffer */
    ret = av_file_map(filename.c_str(), &buffer, &buffer_size, 0, NULL);
    if (ret < 0) {
        printf("Failed to open file!\n");
        return -1;
    }

    /* fill opaque structure used by the AVIOContext read callback */
    bd.base = buffer;
    bd.total_size = buffer_size;
#endif

    // already allocated in member variable
    auto format_ptr = fmt_ctx.get();

    avio_ctx_buffer = (uint8_t *)av_malloc(avio_ctx_buffer_size);
    if (!avio_ctx_buffer) {
        printf("Failed to alloc / cast avio_ctx_buffer!\n");
        return -1;
    }

    avio_ctx = avio_alloc_context(
        avio_ctx_buffer,
        avio_ctx_buffer_size,
        0,
        &bd,
        &read_packet,
        NULL,
        &seek
    );
    if (!avio_ctx) {
        printf("Failed to alloc avio_ctx!\n");
        return -1;
    }
    this->avio_ctx.reset(avio_ctx);

    fmt_ctx->pb = avio_ctx;

    ret = avformat_open_input(&format_ptr, NULL, NULL, NULL);
    if (ret < 0) {
        printf("Could not open input\n");
        return -1;
    }

    if ((ret = avformat_find_stream_info(format_ptr, NULL)) < 0) {
        printf("Cannot find stream information\n");
        return ret;
    }

    /* select the video stream */
    ret = av_find_best_stream(format_ptr, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
    if (ret < 0) {
        printf("Cannot find a video stream in the input file\n");
        return ret;
    }
    video_stream_index = ret;

    /* create decoding context */
    dec_ctx = make_decoder_ptr(dec);
    auto dec_ptr = dec_ctx.get();
    if (!dec_ctx) {
        printf("Failed to create decoder");
        return AVERROR(ENOMEM);
    }

    avcodec_parameters_to_context(dec_ptr, fmt_ctx->streams[video_stream_index]->codecpar);
    time_base = av_q2d(fmt_ctx->streams[video_stream_index]->time_base);

    /* init the video decoder */
    if ((ret = avcodec_open2(dec_ptr, dec, NULL)) < 0) {
        printf("Cannot open video decoder\n");
        return ret;
    }

    aspect_ratio = AspectRatio(dec_ptr->width, dec_ptr->height);

    state = VIDEO_SET_NOT_PLAYED;

    total_duration = chrono::duration<float>(fmt_ctx->duration / AV_TIME_BASE);
    total_duration_str = duration_to_string(total_duration);

    return 0;
}

bool VideoPlayer::load_more_frames() {
    if (av_read_frame(fmt_ctx.get(), packet.get()) < 0) return false;

    int frames_queue_size = frames_queue.size();
    int ret;
    while (frames_queue_size == frames_queue.size() && packet->stream_index == video_stream_index) {
        ret = avcodec_send_packet(dec_ctx.get(), packet.get());

        /*
         * Normally beacuse of this
         * no inifinite loop is possible
         */
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Error while sending a packet to the decoder\n");
            return false;
        }

        while (ret >= 0) {
            /*
             * Allocates new frame each time
             */
            /* If we are skipping frames we don't need
             * to allocate new space for frames that will
             * be just skipped
             */
            auto frame_ptr = make_frame_ptr();
            ret = avcodec_receive_frame(dec_ctx.get(), frame_ptr.get());

            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                break;
            } else if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
                return false;
            }
            if (frame_ptr->width > 0 && frame_ptr->height > 0) frames_queue.push_back(frame_ptr);
        }
    }

    av_packet_unref(packet.get());

    return true;
}

double VideoPlayer::front_frame_timestamp_in_seconds() {
    return ((double)frames_queue.front()->pts) * time_base;
}

frame_ptr VideoPlayer::operator()() {
    // no locks / mutexes needed below as state is atomic
    // and no action will be done until skipping thread is working
    if (is_loading()) return last_frame;

    if (state == VIDEO_NOT_SET) {
        printDebug("No video set, can't play!");
        return nullptr;
    }

    if (pause.paused_now && last_frame) return last_frame;
    if (frames_queue.empty() && !load_more_frames()) {
        // File ended
        played_duration = total_duration;
        return last_frame;
    }

    auto now = chrono::system_clock::now();

    if (state == VIDEO_SET_NOT_PLAYED) {
        state = VIDEO_PLAYING;
        start_time = now;
    }

    if (pause.adjust_player) {
        start_time += now - pause.pause_time;
        pause.adjust_player = false;
    }

    played_duration = duration_diff(now, start_time);

    if (!frames_queue.empty()) {
        auto frame = frames_queue.front();
        auto current = front_frame_timestamp_in_seconds();
        auto expected = played_duration.count();
        // printf("current = %f, expected = %f\n", current, expected);
        if (current <= expected) {
            frames_queue.pop_front();
            last_frame = frame;
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

int VideoPlayer::seek_ts(int64_t &ts) {
    return avformat_seek_file(fmt_ctx.get(), video_stream_index, 0, ts, ts, AVSEEK_FLAG_BACKWARD);
}
void VideoPlayer::skip_frames() {
    frames_queue.clear();
    load_more_frames();
}

auto VideoPlayer::cast_to_start_time(::duration d) {
    return chrono::duration_cast<typename decltype(this->start_time)::duration>(d);
}

void VideoPlayer::set_played_duration(const duration &duration) {
    // Duration below 0, exceeds video length -> don't do anything
    if (is_loading() || (duration < chrono::duration<float>(0)) || (duration > total_duration))
        return;

    const auto started_setting = chrono::system_clock::now();

    static auto make_diff = [](::duration a, ::duration b) {
        return chrono::duration_cast<typename decltype(this->start_time)::duration>(a - b);
    };

    // old frames are now invalid
    frames_queue.clear();
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

    state = SETTING_PLAYED_DURATION;
    auto duration_setting_thread = thread([&, duration, started_setting]() {
        double duration_count = chrono::duration<double>(duration).count();
        int64_t ts =
            av_rescale_q(duration_count, {1, 1}, fmt_ctx->streams[video_stream_index]->time_base);

        // initial
        if (seek_ts(ts) < 0) return;
        load_more_frames();

        played_duration_mutex.lock();
        auto old_played_duration = played_duration;
        played_duration = duration;
        played_duration_mutex.unlock();

        if (duration < old_played_duration) {
            while (front_frame_timestamp_in_seconds() > duration_count) {
                frames_queue.clear();
                skip_frames();
            }
        }

        // skip to real seeked time
        while (front_frame_timestamp_in_seconds() < duration_count) skip_frames();

        if (is_loading()) {
            state = VIDEO_PLAYING;
            const auto ended_setting = chrono::system_clock::now();
            start_time += cast_to_start_time(ended_setting - started_setting);
        }
    });
    duration_setting_thread.detach();
}

bool VideoPlayer::is_loading() { return state == SETTING_PLAYED_DURATION; }
