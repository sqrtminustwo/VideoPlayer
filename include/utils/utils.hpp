#ifndef UTILS_H
#define UTILS_H

#include "types/types.hpp"
#include <string>

void printDebug(std::string msg);

std::string duration_to_string(duration);

// initial window dim, so icons do not become bigger on resize (looks goofy)
std::pair<float, float> get_window_dim();

#endif
