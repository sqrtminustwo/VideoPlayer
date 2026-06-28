#ifndef COMPONENT_H
#define COMPONENT_H

#include "imgui.h"
namespace Overlay {

struct Component {
    virtual void operator()() = 0;

    virtual ~Component() = default;

  protected:
    bool open = true;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
                                    ImGuiWindowFlags_AlwaysAutoResize |
                                    ImGuiWindowFlags_NoSavedSettings |
                                    ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
};

} // namespace Overlay

#endif
