#ifndef FILE_FETCHER_H
#define FILE_FETCHER_H

// For debugging

#include "fetcher/data_fetcher.hpp"
#include <cstddef>

struct FileFetcher : public DataFetcher {
    uint8_t *file = nullptr;
    size_t file_size;

    int getTotalSizeLocal() override;
    void fetchFramesLocal(int_type offset, uint8_t *buf, int_type length) override;

    ~FileFetcher() override;
};

#endif
