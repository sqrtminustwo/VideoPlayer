#ifndef CHRONO_TYPES_H
#define CHRONO_TYPES_H

/*
 * Chrono is very heavy and types.hpp is included almost everywhere
 * separating chrono types results in faster compilation time
 */

#include <chrono>

using time_point = std::chrono::system_clock::time_point;
using duration = std::chrono::duration<float>;

#endif
