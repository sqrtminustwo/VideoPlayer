#include "ffmpeg/stream/stream.hpp"
#include "types/frame/frame_ptr.hpp" // IWYU pragma: keep
#include "types/types.hpp"
#include <iostream>
#include <libavutil/avutil.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

using namespace std;

Stream::Stream(::format_ptr fmt_ctx, LoadStatus status, AVMediaType type)
    : fmt_ctx{fmt_ctx}, status_on_load{status} {
    int ret;
    const AVCodec *dec;

    auto type_str = '\"' + string{av_get_media_type_string(type)} + '\"';

    /* select the video stream */
    if ((ret = av_find_best_stream(fmt_ctx.get(), type, -1, -1, &dec, 0)) < 0) {
        cout << "Cannot find stream of type " << type_str << " in the input file\n";
        return;
    }
    stream_index = ret;

    /* create decoding context */
    dec_ctx = make_decoder_ptr(dec);
    if (!dec_ctx) {
        cout << "Failed to create decoder for type " << type_str << "\n";
        return;
    }

    auto dec_ptr = dec_ctx.get();

    if ((ret = avcodec_parameters_to_context(dec_ptr, get_stream()->codecpar)) < 0) {
        printf("Failed to initialize decoder\n");
        return;
    }

    /* init the video decoder */
    if ((ret = avcodec_open2(dec_ptr, dec, NULL)) < 0) {
        cout << "Cannot open decoder for type " << type_str << "\n";
        return;
    }
}

void Stream::flush_buffers() { avcodec_flush_buffers(dec_ctx.get()); }

decoder_ptr Stream::make_decoder_ptr(const AVCodec *dec) const {
    // Initial initialization
    if (dec == nullptr) return decoder_ptr(nullptr, [](AVCodecContext *) {});

    return decoder_ptr(avcodec_alloc_context3(dec), [](AVCodecContext *f) {
        avcodec_free_context(&f);
    });
}

AVStream *Stream::get_stream() const {
    if (!fmt_ctx || stream_index < 0) return nullptr;
    return fmt_ctx->streams[stream_index];
}
bool Stream::is_valid() const { return get_stream() != nullptr; }
