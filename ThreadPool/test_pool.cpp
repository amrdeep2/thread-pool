#ifdef THREAD_POOL_TESTS

#include "thread_pool.hpp"

#include <atomic>
#include <cassert>
#include <chrono>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace {

void testReturnsFutureValues() {
    ThreadPool pool(4);
    std::vector<std::future<int>> results;

    for (int i = 0; i < 32; ++i) {
        results.push_back(pool.submit([i]() {
            return i * i;
        }));
    }

    for (int i = 0; i < 32; ++i) {
        assert(results[i].get() == i * i);
    }
}

void testVoidTasksRun() {
    ThreadPool pool(2);
    std::atomic<int> counter{0};
    std::vector<std::future<void>> results;

    for (int i = 0; i < 50; ++i) {
        results.push_back(pool.submit([&counter]() {
            counter.fetch_add(1, std::memory_order_relaxed);
        }));
    }

    for (auto& result : results) {
        result.get();
    }

    assert(counter.load(std::memory_order_relaxed) == 50);
}

void testTaskExceptionsReachFuture() {
    ThreadPool pool(1);

    auto result = pool.submit([]() -> int {
        throw std::runtime_error("task failed");
    });

    bool caught = false;
    try {
        (void)result.get();
    }
    catch (const std::runtime_error&) {
        caught = true;
    }

    assert(caught);
}

void testConcurrentSubmission() {
    ThreadPool pool(4);
    std::atomic<int> counter{0};
    std::vector<std::thread> submitters;

    for (int t = 0; t < 4; ++t) {
        submitters.emplace_back([&pool, &counter]() {
            std::vector<std::future<void>> localResults;

            for (int i = 0; i < 25; ++i) {
                localResults.push_back(pool.submit([&counter]() {
                    counter.fetch_add(1, std::memory_order_relaxed);
                }));
            }

            for (auto& result : localResults) {
                result.get();
            }
        });
    }

    for (auto& submitter : submitters) {
        submitter.join();
    }

    assert(counter.load(std::memory_order_relaxed) == 100);
}

void testShutdownRejectsNewTasks() {
    ThreadPool pool(2);
    pool.shutdown();

    bool caught = false;
    try {
        (void)pool.submit([]() {
            return 42;
        });
    }
    catch (const std::runtime_error&) {
        caught = true;
    }

    assert(caught);
}

void testZeroWorkersRejected() {
    bool caught = false;
    try {
        ThreadPool pool(0);
    }
    catch (const std::invalid_argument&) {
        caught = true;
    }

    assert(caught);
}

} // namespace

int main() {
    testReturnsFutureValues();
    testVoidTasksRun();
    testTaskExceptionsReachFuture();
    testConcurrentSubmission();
    testShutdownRejectsNewTasks();
    testZeroWorkersRejected();

    return 0;
}

#endif
