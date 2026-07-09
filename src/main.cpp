#include "ffmpeg/cyclic_fragment_buffer.hpp"

int main() {
    CyclicFragmentBuffer bd{10};
    bd.offset = 0;

    size_t buf_size = 3;
    uint8_t *buf = new uint8_t[buf_size];

    auto rp = [&]() {
        read_packet(&bd, buf, buf_size);
        bd.print();
        print_buf(buf_size, buf);
        print_delimiter();
    };

    bd.print();
    print_delimiter();

    for (int i = 0; i < 10; i++) rp();

    bd.offset = 13;
    rp();

    delete[] buf;
}
