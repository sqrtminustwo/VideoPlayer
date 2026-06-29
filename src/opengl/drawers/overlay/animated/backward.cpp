#include "opengl/drawers/overlay/components/animated/backward.hpp"
#include "IconsFontAwesome4.h"
#include "ffmpeg/video_player.hpp" // IWYU pragma: keep
#include "imgui.h"

Overlay::Backward::Backward(player_ptr player) : Animated{player, "Backward"} {}

const char *Overlay::Backward::get_icon() { return ICON_FA_BACKWARD; }

void Overlay::Backward::set_window_pos() {
    ImVec2 work_size = ImGui::GetMainViewport()->WorkSize;
    ImVec2 window_pos = ImVec2(work_size.x * (1. / 4.), work_size.y / 2);
    ImVec2 window_pos_pivot = ImVec2(0.5f, 0.5f);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
}
