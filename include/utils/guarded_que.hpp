#ifndef GUARDED_QUE_H
#define GUARDED_QUE_H

#include <condition_variable>
#include <atomic>
#include <deque>
#include <mutex>

template <typename T> class GuardedQue {
    const int loading_size_bound;
    std::mutex mutex;
    std::condition_variable cv;
    std::deque<T> queue;
    std::atomic_bool done_waiting = false;

#define LOCK_QUE                                                                                   \
    std::lock_guard<std::mutex> guard { mutex }

  public:
    GuardedQue() = delete;
    GuardedQue(int loading_size_bound) : loading_size_bound{loading_size_bound} {}

    bool empty() {
        LOCK_QUE;
        return queue.empty();
    }

    size_t size() {
        LOCK_QUE;
        return queue.size();
    }

    void push_back(T &&e) {
        LOCK_QUE;
        queue.push_back(std::move(e));
    }

    void clear() {
        LOCK_QUE;
        queue.clear();
        cv.notify_one();
    }

    T &front() {
        LOCK_QUE;
        return queue.front();
    }

    T *front_ptr() {
        LOCK_QUE;
        if (queue.empty()) return nullptr;
        return &queue.front();
    }

    void pop_front() {
        LOCK_QUE;
        queue.pop_front();
        cv.notify_one();
    }

    void stop_waiting() {
        LOCK_QUE;
        done_waiting.store(true, std::memory_order_release);
        cv.notify_one();
    }

    void wait_for_size_change() {
        std::unique_lock<std::mutex> guard{mutex};

        cv.wait(guard, [this] {
            return queue.size() < loading_size_bound ||
                   done_waiting.load(std::memory_order_acquire);
        });

        done_waiting.store(false, std::memory_order_release);
    }
};

#endif
