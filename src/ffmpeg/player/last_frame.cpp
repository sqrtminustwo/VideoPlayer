#include "ffmpeg/player/last_frame.hpp"

frame_ptr LastFrame::get() const { return last_frame; }
void LastFrame::set(frame_ptr &frame) {
    // printf("setting last frame\n");
    last_frame = frame;
    should_send_to_gpu = true;
}

// LastFrame::operator bool() { return last_frame == nullptr; }

void LastFrame::set_to_false() {
    // printf("setting to false\n");
    should_send_to_gpu = false;
}
