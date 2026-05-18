#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

class ThreadPool {
public:
    explicit ThreadPool(size_t threadCount);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template <typename Func, typename... Args>
    auto submit(Func&& task, Args&&... args)
        -> std::future<std::invoke_result_t<Func, Args...>>;

    void shutdown();

private:
    void workerLoop();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;

    bool stopping = false;
};

template <typename Func, typename... Args>
auto ThreadPool::submit(Func&& task, Args&&... args)
    -> std::future<std::invoke_result_t<Func, Args...>> {
    using ReturnType = std::invoke_result_t<Func, Args...>;

    auto packagedTask = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<Func>(task), std::forward<Args>(args)...)
    );

    std::future<ReturnType> result = packagedTask->get_future();

    {
        std::lock_guard<std::mutex> lock(queueMutex);

        if (stopping) {
            throw std::runtime_error("Cannot submit task after shutdown");
        }

        tasks.push([packagedTask]() {
            (*packagedTask)();
        });
    }

    condition.notify_one();

    return result;
}
