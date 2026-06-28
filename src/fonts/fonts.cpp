#include "fonts/fonts.hpp"
#include "IconsFontAwesome4.h"
#include "imgui.h"

void init_imgui_fonts(float scale) {
    ImGuiIO &io = ImGui::GetIO();
    const float baseFontSize = 13.0f * scale;
    io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans.ttf", baseFontSize);
    float iconFontSize =
        baseFontSize * 2.0f / 3.0f; // FontAwesome fonts need to have their sizes reduced
                                    // by 2.0f/3.0f in order to align correctly
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphMinAdvanceX = iconFontSize;
    io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FA, iconFontSize, &icons_config);
}
