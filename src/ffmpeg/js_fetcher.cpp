#ifdef __EMSCRIPTEN__
#include "ffmpeg/js_fetcher.hpp"

extern "C" {
void fetchFrames(int offset, uint8_t *, int length);
int getTotalSize();
}

int JSFetcher::getTotalSizeLocal() { return getTotalSize(); }

void JSFetcher::fetchFramesLocal(int offset, uint8_t *buf, int length) {
    fetchFrames(offset, buf, length);
}
#endif
