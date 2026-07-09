#ifndef CYCLIC_FRAGMENT_BUFFER_H
#define CYCLIC_FRAGMENT_BUFFER_H

#include <cassert>
#include <condition_variable>
#include <mutex>
#include <thread>

// extern "C" {
// void fetchFrames(int offset, int length, uint8_t *);
// int getTotalSize();
// }

int getTotalSize();

void init_zero(int length, uint8_t *buf);

void fetch_frames(int offset, int length, uint8_t *buf);

void print_buf(int buf_size, uint8_t *buf);

void print_delimiter();

struct CyclicFragmentBuffer {
    std::mutex mutex;
    uint8_t *buffer;     // Fixed pointer to the start of buffer memory
    unsigned int head;   // Position of start of valid buffer
    size_t size_present; // Currently amount of usable data present in buffer
    size_t offset;       // Current reading position in total file
    size_t cur_start;    // Start of *base within total file
    size_t size;         // Size of buffer
    size_t total_size;   // Fixed total size of the file

    std::thread filler;

    std::condition_variable cv;
    std::atomic_bool filling = false;

    CyclicFragmentBuffer(unsigned int size);
    ~CyclicFragmentBuffer();

    void refill();
    void refill_full();
    void advance(int);
    void join_filler();
    void print_stats();
    void print();
};

int read_packet(void *opaque, uint8_t *buf, int buf_size);

#endif
