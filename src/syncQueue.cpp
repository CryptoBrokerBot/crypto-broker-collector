#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "types.h"
#include "syncQueue.hpp"

template<class T>
SyncQueue<T>::SyncQueue() {
    count.store(0);
};

template<class T>
void SyncQueue<T>::add(T in) {
    std::unique_lock<std::mutex> lock(mutex);
    the_queue.push(in);
    count.store(count + 1);
    cv.notify_one();
}

template<class T>
T SyncQueue<T>::poll() {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [&]{return the_queue.size();});
    T value = the_queue.front();
    count.store(count - 1);
    the_queue.pop();

    return value;
}

template<class T>
size_t SyncQueue<T>::size() {
    return count;
}