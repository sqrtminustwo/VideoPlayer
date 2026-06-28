#ifndef PAUSE_H
#define PAUSE_H

#include "types/types.hpp"

struct Pause {
    bool adjust_player = false;
    bool paused_now = false;
    time_point pause_time;

    void toggle();
};

#endif
