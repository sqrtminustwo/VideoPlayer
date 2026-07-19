#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <chrono>

constexpr int NUM_OF_TEXTURES = 3;
constexpr int MOUSE_ERROR_X = 20;
constexpr int MOUSE_ERROR_Y = 50;
constexpr auto ZERO_TS = std::chrono::duration<float>(0);
enum TextureChannel { Y = 0, U, V };

#endif
