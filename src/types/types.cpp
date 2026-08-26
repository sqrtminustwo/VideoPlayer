#include "types/types.hpp"

extern "C" {
#include <libavutil/frame.h>
}

std::ostream &operator<<(std::ostream &out, const LoadStatus value) {
    return out << [value] {
#define PROCESS_VAL(p)                                                                             \
    case (p):                                                                                      \
        return #p;
        switch (value) {
            PROCESS_VAL(LOADED_AUDIO);
            PROCESS_VAL(LOADED_VIDEO);
            PROCESS_VAL(NEED_MORE_PACKETS);
            PROCESS_VAL(ERROR);
            PROCESS_VAL(END);
        }
#undef PROCESS_VAL
        return "UNKNOWN";
    }();
}

Resolution::Resolution(int w, int h) : width{w}, height{h} {};
