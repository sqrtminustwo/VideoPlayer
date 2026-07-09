#include <cassert>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include "ffmpeg/cyclic_fragment_buffer.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
}

// extern "C" {
// void fetchFrames(int offset, int length, uint8_t *);
// int getTotalSize();
// }

int getTotalSize() { return 30; }

void init_zero(int length, uint8_t *buf) {
    for (int i = 0; i < length; i++) buf[i] = 0;
}

void fetch_frames(int offset, int length, uint8_t *buf) {
    // printf("fetchFrames(%d, %d)\n", offset, length);
    for (int i = 0; i < length; i++) buf[i] = offset + i;
}

void print_buf(int buf_size, uint8_t *buf) {
    for (int i = 0; i < buf_size; i++) printf("%d ", buf[i]);
    printf("\n");
}

void print_delimiter() { printf("---------------------------------\n"); }

CyclicFragmentBuffer::CyclicFragmentBuffer(unsigned int size) {
    this->size = size;
    this->total_size = getTotalSize();
    size_present = 0;
    buffer = new uint8_t[size];

    refill_full();
}
CyclicFragmentBuffer::~CyclicFragmentBuffer() {
    join_filler();
    delete[] buffer;
}

void CyclicFragmentBuffer::refill() {
    std::lock_guard<std::mutex> lock(mutex);
    auto fill_start = (head + size_present) % size;
    auto offset_start = cur_start + size_present;

    auto fill_size = size - size_present;

    if (fill_size <= 0) return;

    // TODO: abstract trough inheritance
    if (fill_start <= head) fetch_frames(offset_start, fill_size, buffer + fill_start);
    else {
        auto part1 = size - fill_start;
        fetch_frames(offset_start, size - fill_start, buffer + fill_start);
        fetch_frames(offset_start + part1, head, buffer);
    }

    size_present = size;

    filling = false;

    std::cout << "[Background Thread] Done\n";
    cv.notify_all();
}

void CyclicFragmentBuffer::refill_full() {
    // refill_full should not be done in thread
    // thats why we adjust variables without atomics / mutexes
    head = 0;
    cur_start = offset;
    size_present = 0;
    refill();
}

void CyclicFragmentBuffer::advance(int buf_size) {
    offset += buf_size;
    cur_start = offset;
    head = (head + buf_size) % size;
    size_present -= buf_size;
}

void CyclicFragmentBuffer::print_stats() {
    printf(
        "head = %d, size_present = %zu, offset = %zu, cur_start = %zu, size = %zu, "
        "total_size = %zu\n",
        head,
        size_present,
        offset,
        cur_start,
        size,
        total_size
    );
}

void CyclicFragmentBuffer::print() { print_buf(size, buffer); }

void CyclicFragmentBuffer::join_filler() {
    if (filler.joinable()) filler.join();
}

int read_packet(void *opaque, uint8_t *buf, int buf_size) {
    auto *bd = (struct CyclicFragmentBuffer *)opaque;

    // If filling thread has the lock the first line will stop threading errors
    // Ifspurious wakeup filling thread does not yet have lock second line will
    // While loop fixes spurious wakeup
    std::unique_lock<std::mutex> lock(bd->mutex);
    while (bd->filling) bd->cv.wait(lock);

    assert(bd->size >= buf_size);

    int bytes_left = bd->total_size - bd->offset;
    buf_size = FFMIN(buf_size, bytes_left);

    // Needed because function pointer is passed to ffmpeg function
    if (buf_size <= 0) return AVERROR_EOF; // End of file

    auto ahead = bd->offset < bd->cur_start;
    auto behind = bd->cur_start + bd->size < bd->offset;

    if (ahead || behind) {
        // TODO: reuse valid bytes
        lock.unlock();
        bd->refill_full();
        lock.lock();
    }

    // Assumes necessary data is present

    auto start_in_buffer = (bd->offset - bd->cur_start) + bd->head;
    start_in_buffer %= bd->size;

    auto end_in_buffer = start_in_buffer + buf_size;
    end_in_buffer %= bd->size;

    if (end_in_buffer > start_in_buffer) memcpy(buf, bd->buffer + start_in_buffer, buf_size);
    else {
        auto part1 = bd->size - start_in_buffer;
        auto part2 = buf_size - part1;

        memcpy(buf, bd->buffer + start_in_buffer, part1);
        memcpy(buf + part1, bd->buffer, part2);
    }

    bd->print_stats();

    bd->advance(buf_size);

    bd->print_stats();

    if (bd->size_present <= bd->size / 2) {
        bd->join_filler();
        bd->filling = true;
        bd->filler = std::thread(&CyclicFragmentBuffer::refill, bd);
    }

    return buf_size;
}
