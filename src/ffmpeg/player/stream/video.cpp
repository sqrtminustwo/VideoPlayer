#include "ffmpeg/player/stream/video.hpp"
#include "types/frame/frame_ptr.hpp"

extern "C" {
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
}

Video::Video(format_ptr f) : Stream{f} {}

void Video::add_frame(frame_ptr &&frame_ptr) {
    if (frame_ptr->width > 0 && frame_ptr->height > 0) frames_queue.push_back(std::move(frame_ptr));
}

frame_ptr Video::make_black_frame_ptr(Resolution res) {
    int ret;
    auto pix_fmt = is_valid() ? dec_ctx->pix_fmt : AV_PIX_FMT_YUV420P;
    auto w = res.width;
    auto h = res.height;
    ptrdiff_t linesizes1[4];
    int linesizes[4];

    frame_ptr frame{[](AVFrame *f) {
        if (!f->data[0]) return;
        av_freep(&f->data[0]);
    }};
    frame->width = w;
    frame->height = h;
    frame->format = pix_fmt;

    ret = av_image_fill_linesizes(linesizes, pix_fmt, w);
    if (ret < 0) return nullptr;
    ret = av_image_alloc(frame->data, linesizes, w, h, pix_fmt, 4);
    if (ret < 0) return nullptr;
    for (int i = 0; i < 4; i++) linesizes1[i] = linesizes[i];

    av_image_fill_black(
        frame->data,
        linesizes1,
        pix_fmt,
        AVCOL_RANGE_MPEG,
        frame->width,
        frame->height
    );

    return frame;
}
