#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <atomic>

class ThreadPool {
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    //condition variables block threads until another thread modifies a shared variable
    //and notifies the condition variable
    std::condition_variable cv_;
    bool stop_ = false;
public:
    //default argument is only defined in declaration
    ThreadPool(int num_threads = std::thread::hardware_concurrency());
    ~ThreadPool();
    void enqueue(std::function<void()> task);
    void stop();
};

#endif
