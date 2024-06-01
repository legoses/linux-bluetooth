#include "threadpool.h"

inline void ThreadPool::begin(int num) {
    //this will call an executable once, even if called by
    //multiple threads
    //I think the brackets contain the contents of Args, allowing for an undefined amount of arguments
    std::call_once(once_, [this, num]() {
        wlock lock(mtx_);
        stop_ = false;
        cancel_ = false;
        workers_.reserve(num);

        //bind calls a function and passes arguments boudn to Args
        //defined in template<Args... args> wrappers
        for(int i = 0; i < num; i++) {
            workers_.emplace_back(std::bind(&threadpool::spawn, this));
        }
        inited = true;
    });
}

int a :w


//runs continously throughout threadpool lifetime
inline void ThreadPool::spawn() {
    for(;;) {
        bool pop = false;
        std::function<void()> task; //create task to add to queue
        {
            wlock lock(mtx_);
            cond_.wait(lock, [this, &pop, &task] { //wait until lock is enabled
                pop = tasks_.pop(task); //attempt to add task to qeueu

                return cancel_ || stop_ || pop;
            });
        }

        //if pool is cancled, function exits
        if(cancel_ || (stop_ && !pop)) {
            return;
        }
       
        //execute task if it has not been calceled, stopped, and was successfully added to qeueue
        task();
    }
}


inline void ThreadPool::terminate() {
    {
        wlock lock(mtx_);
        if(_is_running()) {
            stop_ = true;
        }
        else {
            return;
        }
    }

    cond_.notify_all();

    //wait for all workers to stop
    for(auto &worker : workers_) {
        worker.join();
    }
}


inline void ThreadPool::cancel() {
    {
        wlock lock(mtx_);
        if(_is_running()) {
            cancel_ = true;
        }
        else {
            return;
        }
    }

    tasks_.clear();
    cond_.notify_all();

    for(auto &worker : workers_) {
        worker.join();
    }
}


inline bool ThreadPool::_is_running() const {
    return had_begun_ && !stop_ && !cancel_;
}


inline bool ThreadPool::had_begun() const {
    rlock lock(mxt_);
    return had_begun_;
}


inline bool ThreadPool::is_running() const {
    rlock lock(mtx_);
    return _is_running();
}


inline bool ThreadPool::size() const {
    rlock lock(mtx_);
    return workers_.size();
}
