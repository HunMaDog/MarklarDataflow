#pragma once

// Marklar headers
#include <attribute.hpp>
#include <error/exception.hpp>
// 3rd party library headers
// C++ standard Library headers
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>

namespace marklar_dataflow
{
namespace policy
{
namespace wait
{

class MARKLAR_EXPORT Timed
{
public:
    Timed(std::atomic<bool> & running)
        : running_(running)
    {}
    virtual ~Timed() noexcept = default;

    std::atomic<std::chrono::nanoseconds> interval() const
    {
        return interval_;
    }

    void set_interval(std::chrono::nanoseconds const & interval)
    {
        interval_ = interval;
    }

protected:
    void preparation()
    {
        if(std::chrono::nanoseconds(0) == interval_)
            throw error::Exception("invalid interval", 0);

        next_weakup_ = std::chrono::high_resolution_clock::now() - interval_;
    }

    void termination()
    {}

    bool is_ready()
    {
        if(!running_)
            return false;

        // Unlock when destructed
        std::unique_lock<std::mutex> lock(mutex_timed_);

        next_weakup_ = next_weakup_ + interval_;

        condition_timed_.wait_until(lock, next_weakup_);

        return running_;
    }

//    std::atomic<std::chrono::nanoseconds> interval_ = std::chrono::nanoseconds(0);
    std::chrono::nanoseconds interval_ = std::chrono::nanoseconds(0);
    std::chrono::time_point<std::chrono::high_resolution_clock> next_weakup_ = std::chrono::high_resolution_clock::now();

private:
    std::atomic<bool> & running_;

    std::mutex mutex_timed_;
    std::condition_variable condition_timed_;
};

} // namespace wait
} // namespace policy
} // namespace marklar_dataflow
