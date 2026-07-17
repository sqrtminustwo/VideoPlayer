#include "ffmpeg/fetcher/file_fetcher.hpp"
#include <cstring>

int FileFetcher::getTotalSizeLocal() { return file_size; }

void FileFetcher::fetchFramesLocal(int offset, uint8_t *buf, int length) {
    memcpy(buf, file + offset, length);
}
