#include "types/frame/last_frame.hpp"

LastFrame::LastFrame() : frame_ptr{} {}

void LastFrame::set_to_false() { should_send_to_gpu = false; }

LastFrame &LastFrame::operator=(frame_ptr &&frame) {
    this->frame = frame.get_real();
    should_send_to_gpu = true;
    return *this;
}
