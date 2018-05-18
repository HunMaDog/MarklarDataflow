#include "gtest/gtest.h"

#include <thread/thread_pool.hpp>

#include <chrono>
#include <iostream>

using marklar_dataflow::thread::ThreadTask;
using marklar_dataflow::thread::ThreadPool;

using namespace std::chrono_literals;

class Timer
{
public:
    Timer()
        : create_time_(std::chrono::high_resolution_clock::now())
    {}

    void step()
    {
        ++counter_;
    }

    uint64_t step_num()
    {
        return counter_;
    }

    uint64_t lifeTime()
    {
        auto end = std::chrono::high_resolution_clock::now();

        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - create_time_).count();
    }

    uint64_t average()
    {
        auto end = std::chrono::high_resolution_clock::now();

        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - create_time_).count() / counter_;
    }

private:
    uint64_t counter_ = 1;
    std::chrono::high_resolution_clock::time_point create_time_;
};

namespace
{

TEST(gtest_thread_thread_pool, thread_pool_size_default)
{
    ThreadPool tp;

    const auto num_of_thread = std::max(std::thread::hardware_concurrency(), 2u) - 1u;


    EXPECT_EQ(tp.thread_number(), num_of_thread);
}

} // namespace
