#ifndef AUDIO_H
#define AUDIO_H

#include "ffmpeg/stream/stream.hpp"

struct Audio : public Stream {
    void add_frame(frame_ptr &&frame) override;

    Audio() = delete;
    Audio(format_ptr);

  private:
    swr_ptr swr_ctx = make_swr_ptr();

    swr_ptr make_swr_ptr(SwrContext * = nullptr);
};

#endif
