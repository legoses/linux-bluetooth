#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include <queue>
#include <mutex>
#include <shared_mutex>

template <typename T>
class BlockingQueue : protected std::queue<T> {
    mutable std::shared_mutex mtx_;

public:
    using wlock = std::unique_lock<std::shared_mutex>; //typedef
    using rlock = std::shared_lock<std::shared_mutex>;
    //allow differentiation between default and user defined constructor. BlockingQueue and BlockingQueue()
    //will call the default or user defined constructor respectivally
    BlockingQueue() = default; 
    ~BlockingQueue() {
        clear();
    }

    //These disable copy and move constructors
    //These constructors would copy or move the contents of a class
    BlockingQueue(const BlockingQueue&) = delete;
    BlockingQueue(BlockingQueue&&) = delete;
    BlockingQueue& operator = (const BlockingQueue&) = delete;
    BlockingQueue& operator = (BlockingQueue&&) = delete;

    bool empty() const;
    size_t size() const;
    void clear();
    void push(const T& obj);

    template <typename... Args>
    void emplace(Args&&... args);

    bool pop(T& holder);
};
#endif
