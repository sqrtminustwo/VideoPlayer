#ifndef SPINNER_ANIMATION_H
#define SPINNER_ANIMATION_H

#include "opengl/drawers/overlay/components/animated/animated.hpp"
#include "types/types.hpp"

namespace Overlay {

struct Spinner : public Animated {
    Spinner(player_ptr);

  private:
    player_ptr player;

    const char *get_icon() override;
    void local_drawer() override;
    virtual bool should_draw() override;
};

} // namespace Overlay

#endif
