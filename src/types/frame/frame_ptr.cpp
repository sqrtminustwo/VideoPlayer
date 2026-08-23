#include "types/frame/frame_ptr.hpp"
#include <memory>

extern "C" {
#include <libavutil/frame.h>
}

frame_ptr::frame_ptr(extra_frame_free extra_free) {
    AVFrame *raw_frame = av_frame_alloc();

    frame = std::shared_ptr<AVFrame>(raw_frame, [extra_free](AVFrame *f) {
        if (!f) return;
        extra_free(f);

        av_frame_free(&f);
    });
}
frame_ptr::frame_ptr(std::nullptr_t) noexcept { frame = nullptr; }

AVFrame &frame_ptr::operator*() const { return *frame; }
AVFrame *frame_ptr::operator->() const { return frame.get(); }
frame_ptr::operator bool() const { return frame != nullptr; }
AVFrame *frame_ptr::get() const { return frame.get(); }
frame_ptr &frame_ptr::operator=(std::nullptr_t) noexcept {
    frame = nullptr;
    return *this;
}
bool frame_ptr::operator==(std::nullptr_t) const noexcept { return frame == nullptr; }
bool frame_ptr::operator!=(std::nullptr_t) const noexcept { return frame != nullptr; }
std::shared_ptr<AVFrame> frame_ptr::get_real() { return frame; }
