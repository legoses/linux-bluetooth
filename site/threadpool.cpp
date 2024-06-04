#include "threadpool.h"

ThreadPool::ThreadPool(int num_threads) {
    //make sure number of threads does not exceed hardware maximum
    for(int i = 0; i < num_threads; i++) {
        threads_.emplace_back([this] {
            while(true) {
                std::function<void()> task;
                //unlock the qeueu before executing a task so other threads can queue tasks
                {
                    std::unique_lock<std::mutex> lock(queue_mutex_);

                    //wait until there is a task to execute or the pool is stopped
                    cv_.wait(lock, [this] {
                        return !tasks_.empty() || stop_;
                    });

                    //if the pool is stopped and there are no tasks remaining, exit the thread
                    if(stop_ && tasks_.empty()) {
                        return;
                    }

                    //get the next task from the queue
                    task = move(tasks_.front());
                    tasks_.pop();
                }
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }

    //unblocks all threads
    cv_.notify_all();

    //wait for thread to finish
    for(auto &thread : threads_) {
        thread.join();
    }
}


void ThreadPool::enqueue(std::function<void ()> task) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        tasks_.emplace(move(task));
    }
    
    //unblock a single waiting thread
    cv_.notify_one();
}
