#include "opengl/drawers/overlay/components/animated/pause.hpp"
#include "IconsFontAwesome4.h"
#include "ffmpeg/video_player.hpp" // IWYU pragma: keep
#include "imgui.h"

Overlay::Pause::Pause(player_ptr player) : Animated{player, "Pause"} {}

const char *Overlay::Pause::get_icon_local(player_ptr player) {
    return player->pause.paused_now ? ICON_FA_PLAY : ICON_FA_PAUSE;
}

const char *Overlay::Pause::get_icon() { return get_icon_local(player); }

void Overlay::Pause::set_window_pos() {
    ImVec2 work_size = ImGui::GetMainViewport()->WorkSize;
    ImVec2 window_pos = ImVec2(work_size.x / 2, work_size.y / 2);
    ImVec2 window_pos_pivot = ImVec2(0.5f, 0.5f);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
}
