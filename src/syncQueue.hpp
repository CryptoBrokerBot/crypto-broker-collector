#pragma once

// #include <iostream>
// #include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

template<class T>
class SyncQueue {
private:
    std::mutex mutex;
    std::atomic<size_t> count;
    std::condition_variable cv;
    std::queue<T> the_queue;
public:
    SyncQueue() {
        count.store(0);
    };

    ~SyncQueue() = default;

    void add(T in) {
        std::unique_lock<std::mutex> lock(mutex);
        the_queue.push(in);
        count.store(count + 1);
        cv.notify_one();
    }

    T poll() {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [&]{return the_queue.size();});
        T value = the_queue.front();
        count.store(count - 1);
        the_queue.pop();

        return value;
    }

    size_t size() {
        return count;
    }

};