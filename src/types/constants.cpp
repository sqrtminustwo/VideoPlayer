#include "types/types.hpp"
#include "types/constants.hpp"

frame_ptr make_empty_frame() {
    return frame_ptr(nullptr, [](AVFrame *) {});
}
