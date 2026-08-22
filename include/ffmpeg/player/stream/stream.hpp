#ifndef STREAM_META_H
#define STREAM_META_H

#include "ffmpeg/fetcher/js_fetcher.hpp"
#include "utils/guarded_que.hpp"
#include "types/types.hpp"
#include <functional>

extern "C" {
#include <libavutil/avutil.h>
}

using frame_que = std::deque<frame_ptr>;
using extra_frame_free = std::function<void(AVFrame *)>;

struct Stream {
    decoder_ptr dec_ctx = make_decoder_ptr();
    int stream_index = -1;

    AVStream *get_stream() const;
    bool is_valid() const;

    virtual int after_init_stream();
    int init_stream(AVMediaType);

    Stream() = delete;
    Stream(format_ptr);
    virtual ~Stream() = default;

    static const int frames_queue_size_bound = 10;
    GuardedQue<frame_ptr> frames_queue{frames_queue_size_bound / 2};

    virtual void add_frame(frame_ptr frame) = 0;
    void frames_queue_operation();

    int seek_ts(const double &);

    static frame_ptr make_frame_ptr(extra_frame_free = [](AVFrame *) {});

  protected:
    format_ptr fmt_ctx;

    decoder_ptr make_decoder_ptr(const AVCodec * = NULL) const;
};

#endif
