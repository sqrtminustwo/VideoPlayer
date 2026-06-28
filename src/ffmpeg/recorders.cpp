#include <algorithm>
#include <string>
#include <vector>

#include "ffmpeg/recorders.hpp"

extern "C" {
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
}

/*
 * A lot of examples of using ffmpeg source C functions can be found in
 * https://ffmpeg.org/doxygen/5.1/dir_687bdf86e8e626c2168c3a2d1c125116.html
 */

using namespace std;

int is_video_input(const AVClass *avclass) {
    if (!avclass) return 0;
    return avclass->category == AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT;
}

int get_formats_devices(std::vector<std::string> &recorders) {
    avdevice_register_all();

    void *opaque;
    const AVInputFormat *ifmt = NULL;
    const char *last_name = "a";
    int recorders_count = 0;

    while (last_name) {
        const char *name = NULL;
        opaque = NULL;

        while ((ifmt = av_demuxer_iterate(&opaque))) {
            if (!is_video_input(ifmt->priv_class)) continue;
            if (find(recorders.begin(), recorders.end(), ifmt->name) == recorders.end()) {
                name = ifmt->name;
                recorders_count++;
                recorders.push_back(name);
            }
        }

        last_name = name;
    }

    return recorders_count;
}
