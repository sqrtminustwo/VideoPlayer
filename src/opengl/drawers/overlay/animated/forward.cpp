#include "opengl/drawers/overlay/components/animated/forward.hpp"
#include "IconsFontAwesome4.h"
#include "player/player.hpp" // IWYU pragma: keep

Overlay::Forward::Forward() : Animated{"Forward"} { x_pos_fraction = 3. / 4.; }

const char *Overlay::Forward::get_icon() { return ICON_FA_FORWARD; }
