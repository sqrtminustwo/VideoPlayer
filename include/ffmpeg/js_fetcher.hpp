#ifdef __EMSCRIPTEN__
#ifndef JS_FETCHER_H
#define JS_FETCHER_H

#include "fetcher/data_fetcher.hpp"

struct JSFetcher : public DataFetcher {
    int getTotalSizeLocal() override;
    void fetchFramesLocal(int offset, uint8_t *buf, int length) override;
};

#endif
#endif
