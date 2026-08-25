#ifndef JS_FETCHER_H
#define JS_FETCHER_H

#include "fetcher/data_fetcher.hpp"

struct JSFetcher : public DataFetcher {
    int getTotalSizeLocal() override;
    void fetchFramesLocal(int_type offset, uint8_t *buf, int_type length) override;
};

#endif
