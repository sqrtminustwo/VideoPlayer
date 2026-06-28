#include "utils/utils.hpp"
#include "imgui.h"
#include "types/types.hpp"
#include <iostream>
#include <iomanip>

void printDebug(std::string msg) {
    printf("------------------------------\n");
    std::cout << msg << "\n";
    printf("------------------------------\n");
}

std::string duration_to_string(duration duration) {
    auto h = std::chrono::duration_cast<std::chrono::hours>(duration);
    duration -= h;
    auto m = std::chrono::duration_cast<std::chrono::minutes>(duration);
    duration -= m;
    auto s = std::chrono::duration_cast<std::chrono::seconds>(duration);

    using namespace std;
    ostringstream out;
    out.fill('0');
    out << setw(2) << h.count() << ":" << std::setw(2) << m.count() << ":" << std::setw(2)
        << s.count();

    return out.str();
}

std::pair<float, float> get_window_dim() {
    float w = ImGui::GetWindowSize().x;
    float h = ImGui::GetContentRegionAvail().y;
    return {w, h};
}
