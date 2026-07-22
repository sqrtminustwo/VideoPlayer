#include "utils/utils.hpp"
#include "imgui.h"
#include "types/types.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

void printDebug(string msg) {
    printf("------------------------------\n");
    cout << msg << "\n";
    printf("------------------------------\n");
}

string duration_to_string(duration duration) {
    using namespace chrono;
    auto h = duration_cast<hours>(duration);
    duration -= h;
    auto m = duration_cast<minutes>(duration);
    duration -= m;
    auto s = duration_cast<seconds>(duration);

    ostringstream out;
    out.fill('0');
    out << setw(2) << h.count() << ":" << setw(2) << m.count() << ":" << setw(2) << s.count();

    return out.str();
}

pair<float, float> get_window_dim() {
    float w = ImGui::GetWindowSize().x;
    float h = ImGui::GetContentRegionAvail().y;
    return {w, h};
}

duration duration_diff(const time_point a, const time_point b) {
    return chrono::duration_cast<chrono::duration<float>>(a - b);
}

time_point now_f() { return chrono::system_clock::now(); }
