#include "opengl/drawers/overlay/components/animated/pause.hpp"
#include "IconsFontAwesome4.h"
#include "ffmpeg/video_player.hpp" // IWYU pragma: keep
#include "imgui.h"
#include "utils/utils.hpp"

Overlay::Pause::Pause(player_ptr player) : Animated{player} {}

const char *Overlay::Pause::get_icon(player_ptr player) {
    return player->pause.paused_now ? ICON_FA_PLAY : ICON_FA_PAUSE;
}

void Overlay::Pause::operator()() {
    ImVec2 work_size = ImGui::GetMainViewport()->WorkSize;
    ImVec2 window_pos = ImVec2(work_size.x / 2, work_size.y / 2);
    ImVec2 window_pos_pivot = ImVec2(0.5f, 0.5f);

    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    ImGui::SetNextWindowBgAlpha(0.f); // Transparent background

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    auto [maxWidth, _] = get_window_dim();
    auto dim = maxWidth / 10;
    ImGui::PushFont(NULL, dim);

    ImGui::Begin("Pause", &open, window_flags | ImGuiWindowFlags_NoResize);

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, opacity), "%s", Overlay::Pause::get_icon(player));

    ImGui::End();

    ImGui::PopStyleVar();
    ImGui::PopFont();
}
