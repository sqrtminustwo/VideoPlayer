#ifndef PAUSE_ANIMATION_H
#define PAUSE_ANIMATION_H

#include "opengl/drawers/overlay/components/animated/animated.hpp"
#include "types/types.hpp"

namespace Overlay {

struct Pause : public Animated {
    Pause(player_ptr);

    void operator()() override;

    static const char *get_icon(player_ptr player);
};

} // namespace Overlay

#endif
