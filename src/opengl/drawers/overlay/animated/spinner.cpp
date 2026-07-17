#include "opengl/drawers/overlay/components/animated/spinner.hpp"
#include "ffmpeg/player/video_player.hpp" // IWYU pragma: keep
#include "imgui.h"
#include "imgui_internal.h"

Overlay::Spinner::Spinner(player_ptr player) : player{player}, Animated{"Spinner"} {}
const char *Overlay::Spinner::get_icon() { return 0; }

// https://github.com/ocornut/imgui/issues/1901
void Overlay::Spinner::local_drawer() {
    unsigned int radius = (icon_size / 2) - 10;
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return;

    ImGuiContext &g = *GImGui;
    const ImGuiStyle &style = g.Style;
    const ImGuiID id = window->GetID("Spinner");

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id)) return;

    // Render
    window->DrawList->PathClear();

    int num_segments = 30;
    int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

    const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
    const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

    const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

    for (int i = 0; i < num_segments; i++) {
        const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
        window->DrawList->PathLineTo(ImVec2(
            centre.x + ImCos(a + g.Time * 8) * radius,
            centre.y + ImSin(a + g.Time * 8) * radius
        ));
    }

    window->DrawList->PathStroke(ImColor(1.f, 1.f, 1.f), 20., 0);
}

bool Overlay::Spinner::should_draw() { return player->is_loading(); }
