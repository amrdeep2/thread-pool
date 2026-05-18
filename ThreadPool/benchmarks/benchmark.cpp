#ifdef THREAD_POOL_BENCHMARK

#include "thread_pool.hpp"

#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;

long long millisecondsSince(Clock::time_point start) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        Clock::now() - start
    ).count();
}

int cpuWork(int value) {
    int result = value;

    for (int i = 0; i < 1000; ++i) {
        result = (result * 31 + i) % 1000003;
    }

    return result;
}

void benchmarkSingleThreaded(int taskCount) {
    auto start = Clock::now();
    long long checksum = 0;

    for (int i = 0; i < taskCount; ++i) {
        checksum += cpuWork(i);
    }

    std::cout << "Single-threaded: " << millisecondsSince(start)
              << " ms, checksum=" << checksum << '\n';
}

void benchmarkThreadPerTask(int taskCount) {
    auto start = Clock::now();
    std::vector<std::thread> threads;
    std::vector<int> results(taskCount);

    threads.reserve(taskCount);

    for (int i = 0; i < taskCount; ++i) {
        threads.emplace_back([i, &results]() {
            results[i] = cpuWork(i);
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    long long checksum = 0;
    for (int value : results) {
        checksum += value;
    }

    std::cout << "Thread-per-task: " << millisecondsSince(start)
              << " ms, checksum=" << checksum << '\n';
}

void benchmarkThreadPool(int taskCount) {
    auto start = Clock::now();
    ThreadPool pool(std::thread::hardware_concurrency() == 0
        ? 4
        : std::thread::hardware_concurrency());

    std::vector<std::future<int>> results;
    results.reserve(taskCount);

    for (int i = 0; i < taskCount; ++i) {
        results.push_back(pool.submit(cpuWork, i));
    }

    long long checksum = 0;
    for (auto& result : results) {
        checksum += result.get();
    }

    std::cout << "Thread pool: " << millisecondsSince(start)
              << " ms, checksum=" << checksum << '\n';
}

} // namespace

int main() {
    constexpr int taskCount = 10000;

    benchmarkSingleThreaded(taskCount);
    benchmarkThreadPerTask(taskCount);
    benchmarkThreadPool(taskCount);

    return 0;
}

#endif
