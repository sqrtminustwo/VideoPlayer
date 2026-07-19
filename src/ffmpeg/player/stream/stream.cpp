#include "ffmpeg/player/stream/stream.hpp"
#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

Stream::Stream(::format_ptr fmt_ctx) : fmt_ctx{fmt_ctx} {}

decoder_ptr Stream::make_decoder_ptr(const AVCodec *dec) {
    // Initial initialization
    if (dec == NULL) return decoder_ptr(nullptr, [](AVCodecContext *) {});

    return decoder_ptr(avcodec_alloc_context3(dec), [](AVCodecContext *f) {
        avcodec_free_context(&f);
    });
}

AVStream *Stream::get_stream() const {
    if (!fmt_ctx || stream_index < 0) return nullptr;
    return fmt_ctx->streams[stream_index];
}

int Stream::init_stream(AVMediaType type, bool required) {
    int ret;
    const AVCodec *dec;

    auto type_str = '\"' + std::string{av_get_media_type_string(type)} + '\"';

    /* select the video stream */
    if ((ret = av_find_best_stream(fmt_ctx.get(), type, -1, -1, &dec, 0)) < 0) {
        std::cout << "Cannot find a stream of type " << type_str << " in the input file\n";
        return required;
    }
    stream_index = ret;

    /* create decoding context */
    dec_ctx = make_decoder_ptr(dec);
    if (!dec_ctx) {
        std::cout << "Failed to create decoder for type " << type_str << "\n";
        return AVERROR(ENOMEM);
    }

    auto dec_ptr = dec_ctx.get();

    avcodec_parameters_to_context(dec_ptr, get_stream()->codecpar);

    /* init the video decoder */
    if ((ret = avcodec_open2(dec_ptr, dec, NULL)) < 0) {
        std::cout << "Cannot open decoder for type " << type_str << "\n";
        return ret;
    }

    return after_init_stream();
}

int Stream::after_init_stream() { return 0; }
