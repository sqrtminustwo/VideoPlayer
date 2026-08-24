#ifndef VIDEO_H
#define VIDEO_H

#include "ffmpeg/player/stream/stream.hpp"

struct Video : public Stream {
    void add_frame(frame_ptr &&frame) override;

    Video() = delete;
    Video(format_ptr);

    frame_ptr make_black_frame_ptr(Resolution);
};

#endif
