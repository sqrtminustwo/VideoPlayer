#include "ffmpeg/platform/make_fetcher.hpp"
#include "ffmpeg/fetcher/file_fetcher.hpp"
#include "types/types.hpp"

#include <memory>

extern "C" {
#include <libavutil/file.h>
}

#if !defined(DEBUG) && !defined(__EMSCRIPTEN__)
#include "buffer/default_buffer.hpp"
#endif

fetcher_ptr make_fetcher_os(const std::string &filename, size_t avio_ctx_buffer_size) {
    int ret;
    uint8_t *buffer = NULL;
    size_t buffer_size;

    /* slurp file content into buffer */
    ret = av_file_map(filename.c_str(), &buffer, &buffer_size, 0, NULL);
    if (ret < 0) {
        printf("Failed to open file!\n");
        return nullptr;
    }

    auto fetcher_l = std::make_unique<FileFetcher>();
#ifdef DEBUG
    fetcher_l->file = buffer;
    fetcher_l->file_size = buffer_size;
#endif
    fetcher_ptr fetcher = std::move(fetcher_l);

#ifdef DEBUG
    init_cyclic_buf(fetcher, avio_ctx_buffer_size);
#else
    fetcher->bd = std::make_unique<DefaultBuffer>();
    fetcher->bd->set_base(buffer);
    fetcher->bd->set_total_size(buffer_size);
#endif

    return fetcher;
}
