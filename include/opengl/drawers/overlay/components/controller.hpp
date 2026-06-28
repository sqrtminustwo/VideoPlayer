#ifndef OVERLAY_DURATION_H
#define OVERLAY_DURATION_H

#include "opengl/drawers/overlay/components/component.hpp"
#include "types/types.hpp"

namespace Overlay {

struct Controller : public Component {
    player_ptr player;

    Controller(player_ptr);

    void operator()() override;
};

} // namespace Overlay

#endif
