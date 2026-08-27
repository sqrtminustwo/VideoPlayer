#ifndef MAKE_FETCHER_H
#define MAKE_FETCHER_H

#include "fetcher/data_fetcher.hpp"

#include <memory>

/*
 * Not in types.hpp because unique_ptr requires
 * size, so we need to include real declaration of data_fetcher
 */
using fetcher_ptr = std::unique_ptr<DataFetcher>;

void init_cyclic_buf(fetcher_ptr &, size_t);

fetcher_ptr make_fetcher(const std::string &filename, size_t);
fetcher_ptr make_fetcher_emscripten(size_t);
fetcher_ptr make_fetcher_os(const std::string &filename, size_t);

#endif
