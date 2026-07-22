#ifndef LAST_FRAME_H
#define LAST_FRAME_H

#include "types/types.hpp"

struct LastFrame {
    bool should_send_to_gpu = false;

    frame_ptr get() const;
    void set(frame_ptr &);

    void set_to_false();

  private:
    frame_ptr last_frame = nullptr;
};

#endif
