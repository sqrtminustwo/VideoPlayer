#include "ffmpeg/actions/pause.hpp"

void Pause::toggle() {
    if (!paused_now) pause_time = std::chrono::system_clock::now();
    adjust_player = paused_now;
    paused_now = !paused_now;
}
