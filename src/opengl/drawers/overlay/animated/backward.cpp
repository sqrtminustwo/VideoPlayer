#include "opengl/drawers/overlay/components/animated/backward.hpp"
#include "IconsFontAwesome4.h"
#include "ffmpeg/player/player.hpp" // IWYU pragma: keep

Overlay::Backward::Backward() : Animated{"Backward"} { x_pos_fraction = 1. / 4.; }

const char *Overlay::Backward::get_icon() { return ICON_FA_BACKWARD; }
