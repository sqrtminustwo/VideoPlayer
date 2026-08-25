#include "ffmpeg/fetcher/file_fetcher.hpp"
#include <cstring>

extern "C" {
#include <libavutil/file.h>
}

FileFetcher::~FileFetcher() {
    uint8_t *base;
    size_t total_size;

    if (file) {
        base = file;
        total_size = file_size;
    } else if (bd->get_base()) {
        base = bd->get_base();
        total_size = bd->get_total_size();
    } else return;

    av_file_unmap(base, total_size - bd->get_offset());
}

int FileFetcher::getTotalSizeLocal() { return file_size; }

void FileFetcher::fetchFramesLocal(int_type offset, uint8_t *buf, int_type length) {
    memcpy(buf, file + offset, length);
}
