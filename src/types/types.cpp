#include "types/types.hpp"

extern "C" {
#include <libavutil/frame.h>
}

Resolution::Resolution(int w, int h) : width{w}, height{h} {};
