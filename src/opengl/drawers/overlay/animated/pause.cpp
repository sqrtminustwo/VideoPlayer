#include "opengl/drawers/overlay/components/animated/pause.hpp"
#include "IconsFontAwesome4.h"
#include "ffmpeg/player/player.hpp" // IWYU pragma: keep

Overlay::Pause::Pause(player_ptr player) : player{player}, Animated{"Pause"} {}

const char *Overlay::Pause::get_icon_local(player_ptr player) {
    return player->pause.paused_now ? ICON_FA_PLAY : ICON_FA_PAUSE;
}

const char *Overlay::Pause::get_icon() { return get_icon_local(player); }
