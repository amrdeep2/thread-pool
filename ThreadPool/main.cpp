#include <iostream>
#include <future>
#include <vector>

#include "thread_pool.hpp"

int main() {
    ThreadPool pool(4);

    std::vector<std::future<int>> results;

    for (int i = 0; i < 10; i++) {
        results.push_back(pool.submit([i]() {
            return i * i;
        }));
    }

    for (auto& result : results) {
        std::cout << "Result: " << result.get() << '\n';
    }

    return 0;
}
