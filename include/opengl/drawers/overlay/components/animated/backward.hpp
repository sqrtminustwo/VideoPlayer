#ifndef BACKWARD_ANIMATION_H
#define BACKWARD_ANIMATION_H

#include "opengl/drawers/overlay/components/animated/animated.hpp"
#include "types/types.hpp"

namespace Overlay {

struct Backward : public Animated {

    Backward(player_ptr);

  private:
    const char *get_icon() override;

  private:
    virtual void set_window_pos() override;
};

} // namespace Overlay

#endif
