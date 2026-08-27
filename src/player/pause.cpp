#include "player/pause.hpp"
#include "utils/utils.hpp"

#define LOCK_MUTEX                                                                                 \
    std::lock_guard<std::mutex> lock { pause_mutex }

void Pause::toggle_adjust_player() {
    LOCK_MUTEX;
    adjust_player_ = !adjust_player_;
}

void Pause::refresh_pause_time_internal() { pause_time_ = now_f(); }
void Pause::refresh_pause_time() {
    LOCK_MUTEX;
    refresh_pause_time_internal();
}

void Pause::toggle() {
    if (!paused_now_) refresh_pause_time_internal();
    adjust_player_ = paused_now_;
    paused_now_ = !paused_now_;
}
