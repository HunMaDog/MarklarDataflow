#pragma once

// Marklar headers
#include <attribute.hpp>
// 3rd party library headers
// C++ standard Library headers
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

// Forward declarations
namespace marklar_dataflow
{
namespace thread
{

class ThreadPool;

} // namespace thread
} // namespace marklar_dataflow

// Classes and structs
namespace marklar_dataflow
{
namespace thread
{

class ThreadTask
{
private:
    friend class ThreadPool;

public:
    ThreadTask(std::function<void()> && func);

    ~ThreadTask() = default;
    ThreadTask(ThreadTask const & rhs) = delete;
    ThreadTask& operator=(ThreadTask const & rhs) = delete;
    ThreadTask(ThreadTask && other) = default;
    ThreadTask& operator=(ThreadTask && other) = default;

    void wait();

    template< class _RepT, class _PeriodT >
    std::cv_status wait_for( std::chrono::duration<_RepT, _PeriodT> const & timeout_duration )
    {
        if(is_avaliable())
            return std::cv_status::no_timeout;

        std::unique_lock<std::mutex> lock(mutex_wait_);

        return condition_wait_.wait_for(lock, timeout_duration);
    }

    bool is_queued() const;
    bool is_progress() const;
    bool is_avaliable() const;

private:
    void execute();

    mutable std::mutex mutex_wait_;
    std::condition_variable condition_wait_;

    std::atomic<bool> queued_;
    std::atomic<bool> progress_;

    std::function<void()> func_;
};

} // namespace thread
} // namespace marklar_dataflow
