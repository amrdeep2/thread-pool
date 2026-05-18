#include "thread_pool.hpp"

#include <stdexcept>

ThreadPool::ThreadPool(size_t threadCount) {
    if (threadCount == 0) {
        throw std::invalid_argument("ThreadPool requires at least one worker thread");
    }

    for (size_t i = 0; i < threadCount; i++) {
        workers.emplace_back(&ThreadPool::workerLoop, this);
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::shutdown() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);

        if (stopping) {
            return;
        }

        stopping = true;
    }

    condition.notify_all();

    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::workerLoop() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            condition.wait(lock, [this]() {
                return stopping || !tasks.empty();
                });

            if (stopping && tasks.empty()) {
                return;
            }

            task = std::move(tasks.front());
            tasks.pop();
        }

        task();
    }
}
