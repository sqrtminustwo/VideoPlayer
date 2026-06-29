#ifndef ANIMATED_H
#define ANIMATED_H

#include "opengl/drawers/overlay/components/component.hpp"
#include "types/types.hpp"

namespace Overlay {

struct Animated : public Component {
    float opacity = 1.f;
    player_ptr player;

    void operator()() override;

    Animated(player_ptr, std::string);

  protected:
    virtual void set_window_pos() = 0;

    virtual const char *get_icon() = 0;

  private:
    std::string name;
};

} // namespace Overlay

#endif
