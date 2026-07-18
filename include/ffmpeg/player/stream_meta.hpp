#ifndef STREAM_META_H
#define STREAM_META_H

#include "ffmpeg/fetcher/js_fetcher.hpp"
#include "types/types.hpp"

extern "C" {
#include <libavutil/avutil.h>
}

struct StreamMeta {
    decoder_ptr dec_ctx = make_decoder_ptr();
    int stream_index = -1;

    int init_stream(AVMediaType, bool required);

    StreamMeta() = delete;
    StreamMeta(format_ptr);

  private:
    format_ptr fmt_ctx;

    decoder_ptr make_decoder_ptr(const AVCodec * = NULL);
};

#endif
