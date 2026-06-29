#ifndef FORWARD_ANIMATION_H
#define FORWARD_ANIMATION_H

#include "opengl/drawers/overlay/components/animated/animated.hpp"
#include "types/types.hpp"

namespace Overlay {

struct Forward : public Animated {

    Forward(player_ptr);

  private:
    const char *get_icon() override;

  private:
    virtual void set_window_pos() override;
};

} // namespace Overlay

#endif
