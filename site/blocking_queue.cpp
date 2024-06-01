#include "blocking_queue.h"

bool BlockingQueue::empty() const {
    rlock lock(mtx_);
    return std::queue<T>::empty();
}


size_t BlockingQueue::size() const {
    rlock lock(mtx_);
    return std::queue<T>::size();
}


void BlockingQueue::clear() {
    wlock lock(mtx_);

    while(!std::queue<T>::empty()) {
        std::queue<T>::pop();
    }
}

void BlockingQueue::push(const T& obj) {
    wlock lock(mtx_);
    std::queue<T>::push(obj);
}

void BlockingQueue::emplace(Args&&... args) {
    wlock lock(mtx_);
    std::queue<T>::emplace(std::forward<Args>args...);
}


bool BlockingQueue::pop(T& holder) {
    wlock lock(mtx_);
    if(std::queue<T>::empty()) {
        return false;
    }
    else {
        holder = std::move(std::queue<T>::front());
        std::queue<T>::pop();
        return true;
    }
}
