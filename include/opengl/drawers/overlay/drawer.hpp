#ifndef OVERLAY_DRAWER_H
#define OVERLAY_DRAWER_H

#include "types/types.hpp"

namespace Overlay {

struct Drawer {
    Drawer();
    void operator()(components_container &components);

  private:
    bool should_draw = true;
};

} // namespace Overlay

#endif
