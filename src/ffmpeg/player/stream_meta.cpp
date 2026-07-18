#include "ffmpeg/player/stream_meta.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

StreamMeta::StreamMeta(::format_ptr fmt_ctx) : fmt_ctx{fmt_ctx} {}

decoder_ptr StreamMeta::make_decoder_ptr(const AVCodec *dec) {
    // Initial initialization
    if (dec == NULL) return decoder_ptr(nullptr, [](AVCodecContext *) {});

    return decoder_ptr(avcodec_alloc_context3(dec), [](AVCodecContext *f) {
        avcodec_free_context(&f);
    });
}

int StreamMeta::init_stream(AVMediaType type, bool required) {
    int ret;
    const AVCodec *dec;

    /* select the video stream */
    if ((ret = av_find_best_stream(fmt_ctx.get(), type, -1, -1, &dec, 0)) < 0) {
        printf("Cannot find a stream of type %d in the input file\n", type);
        if (required) return ret;
    }
    stream_index = ret;

    /* create decoding context */
    dec_ctx = make_decoder_ptr(dec);
    if (!dec_ctx) {
        printf("Failed to create decoder for type %d\n", type);
        return AVERROR(ENOMEM);
    }

    auto dec_ptr = dec_ctx.get();

    avcodec_parameters_to_context(dec_ptr, fmt_ctx->streams[stream_index]->codecpar);

    /* init the video decoder */
    if ((ret = avcodec_open2(dec_ptr, dec, NULL)) < 0) {
        printf("Cannot open decoder for type %d\n", type);
        return ret;
    }

    return 0;
}
