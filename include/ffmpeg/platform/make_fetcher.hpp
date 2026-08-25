#ifndef MAKE_FETCHER_H
#define MAKE_FETCHER_H

#include "types/types.hpp"

void init_cyclic_buf(fetcher_ptr &, size_t);

fetcher_ptr &&make_fetcher(const std::string &filename, size_t);
fetcher_ptr &&make_fetcher_emscripten(size_t);
fetcher_ptr &&make_fetcher_os(const std::string &filename, size_t);

#endif
