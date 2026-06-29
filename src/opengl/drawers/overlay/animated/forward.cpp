#include "opengl/drawers/overlay/components/animated/forward.hpp"
#include "IconsFontAwesome4.h"
#include "ffmpeg/video_player.hpp" // IWYU pragma: keep
#include "imgui.h"

Overlay::Forward::Forward(player_ptr player) : Animated{player, "Forward"} {}

const char *Overlay::Forward::get_icon() { return ICON_FA_FORWARD; }

void Overlay::Forward::set_window_pos() {
    ImVec2 work_size = ImGui::GetMainViewport()->WorkSize;
    ImVec2 window_pos = ImVec2(work_size.x * (3. / 4.), work_size.y / 2);
    ImVec2 window_pos_pivot = ImVec2(0.5f, 0.5f);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
}
