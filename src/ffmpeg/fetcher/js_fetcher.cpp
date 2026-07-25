#ifdef __EMSCRIPTEN__
#include "ffmpeg/fetcher/js_fetcher.hpp"

extern "C" {
void fetchFrames(int_type offset, uint8_t *, int_type length);
int getTotalSize();
}

int JSFetcher::getTotalSizeLocal() { return getTotalSize(); }

void JSFetcher::fetchFramesLocal(int_type offset, uint8_t *buf, int_type length) {
    fetchFrames(offset, buf, length);
}
#endif
