#include "ffmpeg/player/stream/audio.hpp"
#include "types/frame/frame_ptr.hpp"

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libswresample/swresample.h>
}

const auto format = AV_SAMPLE_FMT_FLT;

Audio::Audio(format_ptr f) : Stream{f} {}

int Audio::after_init_stream() {
    if (!is_valid()) {
        printf("No audio\n");
        return -1;
    }

    int ret;
    SwrContext *swr_ctx{};
    auto codecpar = get_stream()->codecpar;

    if ((ret = swr_alloc_set_opts2(
             &swr_ctx,
             &codecpar->ch_layout,
             format,
             codecpar->sample_rate,
             &codecpar->ch_layout,
             (AVSampleFormat)codecpar->format,
             codecpar->sample_rate,
             0,
             nullptr
         )) < 0) {
        printf("Could not set resampler options!\n");
        return ret;
    }

    if ((ret = swr_init(swr_ctx)) < 0) {
        printf("Failed to initialize the resampling context\n");
        swr_free(&swr_ctx);
        return ret;
    }

    this->swr_ctx = make_swr_ptr(swr_ctx);

    return 0;
}

swr_ptr Audio::make_swr_ptr(SwrContext *swr_ctx) {
    return swr_ptr(swr_ctx, [](SwrContext *f) {
        if (!f) return;
        swr_free(&f);
    });
}

void Audio::add_frame(frame_ptr &&frame_ptr) {
    class frame_ptr resampled_frame {};

    resampled_frame->sample_rate = frame_ptr->sample_rate;
    resampled_frame->ch_layout = frame_ptr->ch_layout;
    resampled_frame->format = format;

    swr_convert_frame(swr_ctx.get(), resampled_frame.get(), frame_ptr.get());
    frames_queue.push_back(std::move(resampled_frame));
}
