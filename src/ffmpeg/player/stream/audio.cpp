#include "ffmpeg/player/stream/audio.hpp"
#include "types/frame/frame_ptr.hpp"
#include "types/types.hpp"
#include <libavutil/avutil.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libswresample/swresample.h>
}

const auto format = AV_SAMPLE_FMT_FLT;

Audio::Audio(format_ptr f) : Stream{f, LOADED_AUDIO, AVMEDIA_TYPE_AUDIO} {
    if (!is_valid()) {
        printf("No audio\n");
        return;
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
        return;
    }

    if ((ret = swr_init(swr_ctx)) < 0) {
        printf("Failed to initialize the resampling context\n");
        swr_free(&swr_ctx);
        return;
    }

    this->swr_ctx = make_swr_ptr(swr_ctx);
}

swr_ptr Audio::make_swr_ptr(SwrContext *swr_ctx) {
    return swr_ptr(swr_ctx, [](SwrContext *f) {
        if (!f) return;
        swr_free(&f);
    });
}

void Audio::add_frame(frame_ptr &&frame) {
    frame_ptr resampled_frame{};

    resampled_frame->sample_rate = frame->sample_rate;
    resampled_frame->ch_layout = frame->ch_layout;
    resampled_frame->format = format;
    // front_frame_timestamp_in_seconds is based on pts
    resampled_frame->pts = frame->pts;

    swr_convert_frame(swr_ctx.get(), resampled_frame.get(), frame.get());

    frames_queue.push_back(std::move(resampled_frame));
}
