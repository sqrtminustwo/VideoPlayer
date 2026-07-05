#include "opengl/drawers/overlay/components/animated/animated.hpp"

Overlay::Animated::Animated(std::string name) : name{name} {}

void Overlay::Animated::operator()() {
    if (!should_draw()) return;

    set_window_pos();
    ImGui::SetNextWindowBgAlpha(0.f); // Transparent background
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::PushFont(NULL, icon_size);

    ImGui::Begin(name.c_str(), &open, window_flags | ImGuiWindowFlags_NoResize);

    local_drawer();

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopFont();
}

void Overlay::Animated::local_drawer() {
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, opacity), "%s", get_icon());
}

void Overlay::Animated::set_window_pos() {
    ImVec2 work_size = ImGui::GetMainViewport()->WorkSize;
    ImVec2 window_pos = ImVec2(work_size.x / 2, work_size.y / 2);
    ImVec2 window_pos_pivot = ImVec2(0.5f, 0.5f);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
}

bool Overlay::Animated::should_draw() { return opacity > 0; }
