cmake_minimum_required(VERSION 3.15)

project(ThreadPoolTaskScheduler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(thread_pool
    ThreadPool/thread_pool.cpp
)

target_include_directories(thread_pool
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool
)

add_executable(thread_pool_demo
    ThreadPool/main.cpp
)

target_link_libraries(thread_pool_demo
    PRIVATE
        thread_pool
)

add_executable(thread_pool_tests
    ThreadPool/test_pool.cpp
)

target_compile_definitions(thread_pool_tests
    PRIVATE
        THREAD_POOL_TESTS
)

target_link_libraries(thread_pool_tests
    PRIVATE
        thread_pool
)

add_executable(thread_pool_benchmark
    ThreadPool/benchmarks/benchmark.cpp
)

target_compile_definitions(thread_pool_benchmark
    PRIVATE
        THREAD_POOL_BENCHMARK
)

target_link_libraries(thread_pool_benchmark
    PRIVATE
        thread_pool
)
