#ifndef PAUSE_H
#define PAUSE_H

#include "types/types.hpp"
#include <mutex>

class Pause {
    std::mutex pause_mutex;
    bool adjust_player_ = false;
    bool paused_now_ = false;
    time_point pause_time_;

    void refresh_pause_time_internal();

  public:
    decltype(adjust_player_) adjust_player() const { return adjust_player_; }
    decltype(paused_now_) paused_now() const { return paused_now_; }
    decltype(pause_time_) pause_time() const { return pause_time_; }

    void toggle();
    void toggle_adjust_player();
    void refresh_pause_time();
};

#endif
