#ifndef THREADSAFEQUEUE_HPP
#define THREADSAFEQUEUE_HPP

#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;

    // Push an element into the queue
    void enqueue(T item) {
        std::unique_lock<std::mutex> lock(mtx);
        q.push(std::move(item));
        cv.notify_one(); // Notify one waiting thread, if any
    }

    // Pop an element from the queue
    T dequeue() {
        std::unique_lock<std::mutex> lock(mtx);
        if (q.empty())
            throw std::runtime_error("Can not dequeue from the empty queue.");

        // cv.wait(lock, [this] { return !q.empty(); }); // Wait until the queue is not empty // todo: remove
        T item = std::move(q.front());
        q.pop();
        return item;
    }

    // Check if the queue is empty
    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx);
        return q.empty();
    }

    // Get current size
    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx);
        return q.size();
    }

private:
    std::queue<T> q;
    mutable std::mutex mtx;
    std::condition_variable cv;
};


#endif //THREADSAFEQUEUE_HPP
