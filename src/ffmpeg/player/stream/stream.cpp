#include "ffmpeg/player/stream/stream.hpp"
#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

Stream::Stream(::format_ptr fmt_ctx) : fmt_ctx{fmt_ctx} {}

int Stream::seek_ts(const double &duration_count) {
    auto time_base = av_q2d(get_stream()->time_base);
    auto ts = static_cast<int64_t>(duration_count / time_base);

    // https://stackoverflow.com/questions/21475397/cannot-get-first-frames-using-avformat-seek-file
    // avformat_seek_file(fmt_ctx.get(), stream_index, 0, ts, ts, AVSEEK_FLAG_BACKWARD);
    int ret = av_seek_frame(fmt_ctx.get(), stream_index, ts, AVSEEK_FLAG_BACKWARD);

    // https://ffmpeg.org/doxygen/trunk/group__lavc__misc.html#gaf60b0e076f822abcb2700eb601d352a6
    avcodec_flush_buffers(dec_ctx.get());

    return ret;
}

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

    if ((ret = avcodec_parameters_to_context(dec_ptr, get_stream()->codecpar)) < 0) {
        printf("Failed to initialize decoder\n");
        return ret;
    }

    /* init the video decoder */
    if ((ret = avcodec_open2(dec_ptr, dec, NULL)) < 0) {
        std::cout << "Cannot open decoder for type " << type_str << "\n";
        return ret;
    }

    return after_init_stream();
}

int Stream::after_init_stream() { return 0; }
