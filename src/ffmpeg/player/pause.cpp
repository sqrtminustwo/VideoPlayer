#include "ffmpeg/player/pause.hpp"
#include "utils/utils.hpp"

void Pause::toggle() {
    if (!paused_now) pause_time = now_f();
    adjust_player = paused_now;
    paused_now = !paused_now;
}
