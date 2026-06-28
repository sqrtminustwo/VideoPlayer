#ifndef OVERLAY_DRAWER_H
#define OVERLAY_DRAWER_H

#include "types/types.hpp"

namespace Overlay {

struct Drawer {
    void operator()(bool *overlay_open, components_vector &components);
};

} // namespace Overlay

#endif
