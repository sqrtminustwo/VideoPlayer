#include "opengl/drawers/overlay/components/controller.hpp"
#include "opengl/drawers/overlay/components/animated/pause.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "types/constants.hpp"
#include "utils/utils.hpp"
#include "ffmpeg/player/player.hpp" // IWYU pragma: keep

Overlay::Controller::Controller(player_ptr player) : player(player){};

bool mouseInBound(ImVec2 left_upper, ImVec2 right_bottom, float *value) {
    if (!ImGui::IsMousePosValid()) return false;
    const auto mouse_pos = ImGui::GetIO().MousePos;

    left_upper.y -= MOUSE_ERROR_Y;
    right_bottom.y += MOUSE_ERROR_Y;

    auto fits_y = (mouse_pos.y >= left_upper.y - MOUSE_ERROR_Y) &&
                  (mouse_pos.y <= right_bottom.y + MOUSE_ERROR_Y);
    auto fits_x = (left_upper.x <= mouse_pos.x) && (mouse_pos.x <= right_bottom.x);

    if (fits_y && fits_x) {
        float size = right_bottom.x - left_upper.x;
        float mouse_on_len = mouse_pos.x - left_upper.x;
        *value = mouse_on_len / size;
        return true;
    }

    return false;
}

void Overlay::Controller::operator()() {
    // Set window parameters
    ImGuiIO &io = ImGui::GetIO();

    const float PAD = 10.0f;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos;
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos = ImVec2((work_size.x / 2) + work_pos.x, work_pos.y + work_size.y - PAD);
    ImVec2 window_pos_pivot = ImVec2(0.5f, 1.0f);

    auto maxWidth = work_size.x * 0.985;
    auto maxHeight = 90;

    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    ImGui::SetNextWindowSize(ImVec2(maxWidth, maxHeight));

    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    ImGui::Begin("Controller", &open, window_flags);

    ImGui::BeginGroup();

    ImGuiWindow *window = ImGui::GetCurrentWindow();
    const float center_y = window->Pos.y + (maxHeight / 2.0f);

    auto get_pos_centered_y = [&window, &center_y](float obj_h, bool text = false) {
        float y = center_y - (obj_h / 2.);
        if (text) y -= window->DC.CurrLineTextBaseOffset;
        return ImVec2(window->DC.CursorPos.x, y);
    };

    // Initialize duration str to reserve the sapce at the end

    auto duration_str = std::format(
        "{} / {}",
        duration_to_string(player->played_duration),
        player->get_total_duration_str()
    );
    auto [duration_width, duration_height] = ImGui::CalcTextSize(duration_str.c_str());

    float pauseButtonWidth = 50;
    float pauseButtonHeight = 50;
    maxWidth = maxWidth - pauseButtonWidth - duration_width - ImGui::GetStyle().FramePadding.x * 2 -
               ImGui::GetStyle().ItemSpacing.x * 2;

    // Pause button
    ImGui::SetCursorScreenPos(get_pos_centered_y(pauseButtonHeight));
    if (ImGui::Button(
            Overlay::Pause::get_icon_local(player),
            ImVec2(pauseButtonWidth, pauseButtonHeight)
        ))
        player->pause.toggle();

    // Progress bar
    ImGui::SameLine();
    const ImU32 fg_col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
    const ImU32 bg_col = ImGui::GetColorU32(ImGuiCol_Button);
    const float value = player->played_duration.count() / player->get_total_duration().count();
    ImVec2 size = ImVec2(maxWidth, 6);
    size.x -= ImGui::GetStyle().FramePadding.x * 2;

    ImVec2 pos = get_pos_centered_y(size.y);

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(bb); // Reserves layout space horizontally

    // float clicked_duration_ratio = 0.0f;
    // if (ImGui::IsMouseClicked(0) && mouseInBound(bb.Min, bb.Max, &clicked_duration_ratio)) {
    //     auto clicked_duration = player->get_total_duration() * clicked_duration_ratio;
    //     player->set_played_duration(clicked_duration);
    // }

    ImGui::GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max, bg_col);
    auto value_pos = ImVec2(pos.x + size.x * value, bb.Max.y);
    ImGui::GetWindowDrawList()->AddRectFilled(bb.Min, value_pos, fg_col);
    ImGui::GetWindowDrawList()->AddCircleFilled(
        ImVec2(value_pos.x, bb.Min.y + (bb.Max.y - bb.Min.y) / 2),
        10,
        ImColor(1.f, 1.f, 1.f)
    );

    // Duration text
    ImGui::SameLine();
    ImVec2 text_pos = get_pos_centered_y(duration_height, true);

    const ImRect text_bb(
        text_pos,
        ImVec2(text_pos.x + duration_width, text_pos.y + duration_height)
    );
    ImGui::ItemSize(text_bb); // Reserves layout space horizontally

    ImGui::GetWindowDrawList()
        ->AddText(text_pos, ImGui::GetColorU32(ImGuiCol_Text), duration_str.c_str());

    ImGui::EndGroup();
    ImGui::End();
}
