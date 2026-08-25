#include "ffmpeg/platform/make_fetcher.hpp"
#include "ffmpeg/fetcher/js_fetcher.hpp"
#include "types/types.hpp"
#include <memory>

extern "C" {
#include <libavutil/file.h>
}

fetcher_ptr &&make_fetcher_emscripten(size_t avio_ctx_buffer_size) {
    fetcher_ptr fetcher = std::make_unique<JSFetcher>();
    init_cyclic_buf(fetcher, avio_ctx_buffer_size);

    return std::move(fetcher);
}
