#include "opengl/drawers/overlay/components/animated/animated.hpp"

Overlay::Animated::Animated(player_ptr player, std::string name) : player{player}, name{name} {}

void Overlay::Animated::operator()() {
    set_window_pos();
    ImGui::SetNextWindowBgAlpha(0.f); // Transparent background

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::PushFont(NULL, 100);

    ImGui::Begin(name.c_str(), &open, window_flags | ImGuiWindowFlags_NoResize);

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, opacity), "%s", get_icon());

    ImGui::End();

    ImGui::PopStyleVar();
    ImGui::PopFont();
}
