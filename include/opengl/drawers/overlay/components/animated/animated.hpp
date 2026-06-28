#ifndef ANIMATED_H
#define ANIMATED_H

#include "opengl/drawers/overlay/components/component.hpp"
#include "types/types.hpp"

namespace Overlay {

struct Animated : public Component {
    float opacity = 0.f;
    player_ptr player;

    Animated(player_ptr);
};

} // namespace Overlay

#endif
