#include "ffmpeg/player/last_frame.hpp"

void LastFrame::set(frame_ptr &frame) {
    last_frame = frame;
    should_send_to_gpu = true;
}

frame_ptr LastFrame::get() const { return last_frame; }

// LastFrame::operator bool() { return last_frame == nullptr; }

void LastFrame::set_to_false() { should_send_to_gpu = false; }
