#ifndef STREAM_META_H
#define STREAM_META_H

#include "utils/guarded_que.hpp"
#include "types/types.hpp"

extern "C" {
#include <libavutil/avutil.h>
}

struct Stream {
    decoder_ptr dec_ctx = make_decoder_ptr();
    int stream_index = -1;
    const LoadStatus status_on_load;

    AVStream *get_stream() const;
    bool is_valid() const;

    Stream() = delete;
    Stream(const Stream &) = delete;
    Stream(format_ptr, LoadStatus, AVMediaType);
    virtual ~Stream() = default;

    static const int frames_queue_size_bound = 10;
    GuardedQue<frame_ptr> frames_queue{frames_queue_size_bound / 2};

    virtual void add_frame(frame_ptr &&frame) = 0;
    void frames_queue_operation();

    void flush_buffers();

  protected:
    format_ptr fmt_ctx;

    decoder_ptr make_decoder_ptr(const AVCodec * = NULL) const;
};

#endif
