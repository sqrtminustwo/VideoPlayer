#ifndef ANIMATED_COMPONENTS_HANDLE_H
#define ANIMATED_COMPONENTS_HANDLE_H

#include "opengl/drawers/overlay/components/animated/animated.hpp"

namespace Overlay {

struct AnimatedComponentsHandle {
    AnimatedComponentsHandle();

    void add_component(Animated);
    void remove_component(Animated);
};

} // namespace Overlay

#endif
