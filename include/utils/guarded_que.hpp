#ifndef GUARDED_QUE_H
#define GUARDED_QUE_H

#include <deque>
#include <mutex>

template <typename T> class GuardedQue {
    std::mutex mutex;
    std::deque<T> queue;

  public:
#define LOCK_QUE                                                                                   \
    std::lock_guard<std::mutex> guard { mutex }

    bool empty() {
        LOCK_QUE;
        return queue.empty();
    }

    size_t size() {
        LOCK_QUE;
        return queue.size();
    }

    void push_back(T e) {
        LOCK_QUE;
        queue.push_back(e);
    }

    void clear() {
        LOCK_QUE;
        queue.clear();
    }

    T &front() {
        LOCK_QUE;
        return queue.front();
    }

    void pop_front() {
        LOCK_QUE;
        return queue.pop_front();
    }
};

#endif
