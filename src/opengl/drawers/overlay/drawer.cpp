#include "opengl/drawers/overlay/drawer.hpp"
#include "opengl/drawers/overlay/components/component.hpp" // IWYU pragma: keep
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

Overlay::Drawer::Drawer(std::mutex &components_mutex) : components_mutex{components_mutex} {}

void Overlay::Drawer::operator()(bool *overlay_open, components_vector &components) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (*overlay_open) {
        components_mutex.lock();
        for (auto &component : components) (*component)();
        components_mutex.unlock();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
