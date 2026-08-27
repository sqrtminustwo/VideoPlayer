#ifndef ANIMATED_H
#define ANIMATED_H

#include "opengl/drawers/overlay/components/component.hpp"
#include "types/types.hpp"

#include <atomic>

namespace Overlay {

struct Animated : public Component {
    std::atomic<float> opacity = 0.f;
    static const unsigned int icon_size = 100;
    // Default centered
    float x_pos_fraction = 1. / 2.;

    void operator()() override;

    Animated(std::string);

  protected:
    // Default is centered
    virtual bool should_draw();
    virtual const char *get_icon() = 0;
    virtual void local_drawer();

  private:
    std::string name;
};

} // namespace Overlay

#endif
