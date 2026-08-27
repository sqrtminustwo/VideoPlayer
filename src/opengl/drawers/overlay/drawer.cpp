#include "opengl/drawers/overlay/drawer.hpp"
#include "opengl/drawers/overlay/components/component.hpp" // IWYU pragma: keep
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

Overlay::Drawer::Drawer() {}

void Overlay::Drawer::operator()(components_container &components) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    for (auto &component : components) (*component)();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
