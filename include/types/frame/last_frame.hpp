#ifndef LAST_FRAME_H
#define LAST_FRAME_H

#include "types/frame/frame_ptr.hpp"

struct LastFrame : public frame_ptr {
    LastFrame();

    bool should_send_to_gpu = false;

    void set_to_false();

    LastFrame &operator=(frame_ptr &&);
};

#endif
