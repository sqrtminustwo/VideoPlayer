#ifndef RECORDERS_H
#define RECORDERS_H

// Old header, not used

#include <vector>
#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
}

int is_video_input(const AVClass *avclass);

int get_formats_devices(std::vector<std::string> &recorders);

#endif
