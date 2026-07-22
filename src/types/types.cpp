#include "types/types.hpp"

extern "C" {
#include <libavutil/frame.h>
}

Resolution::Resolution(int w, int h) : width{w}, height{h} {};

frame_ptr make_frame_ptr() {
    return frame_ptr(av_frame_alloc(), [](AVFrame *f) { av_frame_free(&f); });
}
