#ifndef OVERLAY_DRAWER_H
#define OVERLAY_DRAWER_H

#include "types/types.hpp"

namespace Overlay {

struct Drawer {
    Drawer();
    void operator()(bool *overlay_open, components_container &components);
};

} // namespace Overlay

#endif
