# C++ Thread Pool & Task Scheduler

A small C++17 thread pool that executes submitted callables on reusable worker
threads and returns typed `std::future<T>` results to callers.

## Core Ideas

- Fixed worker threads are created once and reused for many tasks.
- Producers submit tasks into a shared queue.
- Worker threads consume tasks using `std::condition_variable` instead of busy
  waiting.
- `std::packaged_task` captures task results and exceptions for `std::future`.
- Shutdown is graceful: workers finish queued tasks, then join safely.

## Layout

```text
thread-pool/
├── CMakeLists.txt
├── README.md
└── ThreadPool/
    ├── thread_pool.hpp
    ├── thread_pool.cpp
    ├── main.cpp
    ├── test_pool.cpp
    ├── benchmarks/
    │   └── benchmark.cpp
    └── ThreadPool.sln
```

## Build With CMake

```sh
cmake -S . -B build
cmake --build build
```

Run the demo:

```sh
./build/thread_pool_demo
```

Run tests:

```sh
./build/thread_pool_tests
```

Run benchmark:

```sh
./build/thread_pool_benchmark
```

On Windows multi-config generators, executables may be under
`build/Debug` or `build/Release`.

## Example

```cpp
ThreadPool pool(4);

auto result = pool.submit([]() {
    return 21 * 2;
});

std::cout << result.get() << '\n';
```

## Resume Bullets

- Built a configurable C++17 thread pool using worker threads, mutexes, and
  condition variables.
- Implemented generic asynchronous task execution using `std::packaged_task`
  and `std::future<T>`.
- Designed graceful shutdown logic with safe synchronization and thread joining.
- Benchmarked thread pool execution against single-threaded and thread-per-task
  approaches.
# thread-pool
