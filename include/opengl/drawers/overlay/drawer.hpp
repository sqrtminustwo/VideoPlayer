#ifndef OVERLAY_DRAWER_H
#define OVERLAY_DRAWER_H

#include "types/types.hpp"

namespace Overlay {

struct Drawer {
    Drawer(std::mutex &components_mutex);
    void operator()(bool *overlay_open, components_vector &components);

  private:
    std::mutex &components_mutex;
};

} // namespace Overlay

#endif
