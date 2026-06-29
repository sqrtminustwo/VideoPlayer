#ifndef PAUSE_ANIMATION_H
#define PAUSE_ANIMATION_H

#include "opengl/drawers/overlay/components/animated/animated.hpp"
#include "types/types.hpp"

namespace Overlay {

struct Pause : public Animated {

    Pause(player_ptr);

    static const char *get_icon_local(player_ptr player);

  private:
    const char *get_icon() override;

  private:
    virtual void set_window_pos() override;
};

} // namespace Overlay

#endif
