#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "types/types.hpp"

constexpr int NUM_OF_TEXTURES = 3;
constexpr int MOUSE_ERROR_X = 20;
constexpr int MOUSE_ERROR_Y = 50;
enum TextureChannel { Y = 0, U, V };
frame_ptr make_empty_frame();

#endif
