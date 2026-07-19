#include "ffmpeg/player/stream/video.hpp"

extern "C" {
#include <libavutil/frame.h>
}

Video::Video(format_ptr f) : Stream{f} {}

void Video::add_frame(frame_ptr frame_ptr) {
    if (frame_ptr->width > 0 && frame_ptr->height > 0) frames_queue.push_back(frame_ptr);
}
