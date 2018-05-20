#include <policy/scheduler/timed.hpp>

// Marklar headers
#include <error/exception.hpp>
// 3rd party library headers
// C++ standard Library headers

using marklar_dataflow::policy::scheduler::Timed;

Timed::Timed(std::atomic<bool> & running)
    : running_(running)
{}

std::chrono::nanoseconds Timed::interval() const
{
    return interval_;
}

void Timed::set_interval(std::chrono::nanoseconds const & interval)
{
    interval_ = interval;
}

void Timed::preparation()
{
    if(std::chrono::nanoseconds(0) == interval_)
        throw error::Exception("invalid interval", 0);

    next_weakup_ = std::chrono::high_resolution_clock::now() - interval_;
}

void Timed::termination()
{}

bool Timed::is_ready()
{
    if(!running_)
        return false;

    std::unique_lock<std::mutex> lock(mutex_timed_);

    next_weakup_ = next_weakup_ + interval_;

    condition_timed_.wait_until(lock, next_weakup_);

    return running_;
}
