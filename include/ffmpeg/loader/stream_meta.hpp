#ifndef STREAM_META_H
#define STREAM_META_H

#include "ffmpeg/fetcher/js_fetcher.hpp"
#include "types/types.hpp"

extern "C" {
#include <libavutil/avutil.h>
}

struct Stream {
    decoder_ptr dec_ctx = make_decoder_ptr();
    int stream_index = -1;

    AVStream *get_stream() const;

    int init_stream(AVMediaType, bool required);

    Stream() = delete;
    Stream(format_ptr);

  private:
    format_ptr fmt_ctx;

    decoder_ptr make_decoder_ptr(const AVCodec * = NULL);
};

#endif
