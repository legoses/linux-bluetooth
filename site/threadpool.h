#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <future>
#include <vector>
#include <functional> 
#include "blocking_queue.h"

class ThreadPool {
    std::vector<std::thread> workers_;
    mutable BlockingQueue<std::function<void()>> tasks_;
public:
    //control interface
    void begin(int num);
    void terminate(); //stop and process all tasks
    void cancel(); //stop and drop all remaining tasks

    bool had_begun() const;
    bool is_running() const;
    int size() const;

    //accepts arbirary functions, return std::future
    //basically async execution
    //look more into templates
    template<class F, class... Args>
    auto async(F&& f, Args&... args) const -> std::future<decltype(f(args...))>;
};

#endif
