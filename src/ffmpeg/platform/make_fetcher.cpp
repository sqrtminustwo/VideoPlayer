#include "ffmpeg/platform/make_fetcher.hpp"
#include "buffer/cfb2.hpp"
#include "fetcher/data_fetcher.hpp"

#include <memory>

extern "C" {
#include <libavutil/file.h>
}

void init_cyclic_buf(fetcher_ptr &fetcher, size_t avio_ctx_buffer_size) {
    fetcher->bd = std::make_unique<CyclicFragmentBuffer2>(
        fetcher.get(),
        avio_ctx_buffer_size * 4,
        avio_ctx_buffer_size * 3,
        avio_ctx_buffer_size,
        FULL
    );
}

fetcher_ptr make_fetcher(const std::string &filename, size_t avio_ctx_buffer_size) {
#ifdef __EMSCRIPTEN__
    return make_fetcher_emscripten(avio_ctx_buffer_size);
#else
    return make_fetcher_os(filename, avio_ctx_buffer_size);
#endif
}
