#ifndef FILE_FETCHER_H
#define FILE_FETCHER_H

// For debugging

#include "fetcher/data_fetcher.hpp"
#include <cstddef>

struct FileFetcher : public DataFetcher {
    uint8_t *file;
    size_t file_size;

    int getTotalSizeLocal() override;
    void fetchFramesLocal(int offset, uint8_t *buf, int length) override;
};

#endif
