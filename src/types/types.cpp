#include "types/types.hpp"

extern "C" {
#include <libavutil/frame.h>
}

AspectRatio::AspectRatio(int numer, int denom) : numer{numer}, denom{denom} {};

frame_ptr make_frame_ptr() {
    return frame_ptr(av_frame_alloc(), [](AVFrame *f) { av_frame_free(&f); });
}
