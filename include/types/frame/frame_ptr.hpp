#ifndef FRAME_H
#define FRAME_H

#include "types/types.hpp"
#include <functional>

using extra_frame_free = std::function<void(AVFrame *)>;

class frame_ptr {
  protected:
    std::shared_ptr<AVFrame> frame;

  public:
    int offset = 0;

    frame_ptr(extra_frame_free extra_free = [](AVFrame *) {});
    frame_ptr(std::nullptr_t) noexcept;

    ~frame_ptr() = default;
    // frame_ptr(const frame_ptr &) = delete;
    frame_ptr(const frame_ptr &) = default;
    frame_ptr &operator=(const frame_ptr &) = default;
    frame_ptr(frame_ptr &&) = default;
    frame_ptr &operator=(frame_ptr &&) = default;
    frame_ptr &operator=(std::nullptr_t) noexcept;

    AVFrame &operator*() const;
    AVFrame *operator->() const;
    explicit operator bool() const;
    AVFrame *get() const;
    std::shared_ptr<AVFrame> get_real();
    bool operator==(std::nullptr_t) const noexcept;
    bool operator!=(std::nullptr_t) const noexcept;
};

#endif
