#pragma once

// Marklar headers
#include <attribute.hpp>
#include <error/exception.hpp>
// 3rd party library headers
// C++ standard Library headers
#include <atomic>
#include <functional>

#include <condition_variable>
#include <mutex>

#include <iostream>

namespace marklar_dataflow
{
namespace policy
{
namespace wait
{

class MARKLAR_EXPORT Trigger
{
public:
    Trigger(std::atomic<bool> & running)
        : running_(running)
    {}
    virtual ~Trigger() noexcept = default;

    void trigger()
    {
        if(!running_)
            return;

// TODO :: multiple call from threads
        std::unique_lock<std::mutex> lock(mutex_triggered_);

        triggered_ = true;

        trigger_function_();

// TODO :: wait from params dat
//        if()
        condition_triggered_.wait(
            lock
            , [this]()
            {
                return !triggered_;
            }
        );
    }

protected:
    void set_trigger_function(std::function<void()> && func)
    {
        trigger_function_ = std::move(func);
    }

    void preparation()
    {}

    void termination()
    {}

    bool is_ready()
    {
        if(triggered_)
        {
            triggered_ = false;
            condition_triggered_.notify_one();

            return running_;
        }

        return false;
    }

private:
    std::atomic<bool> & running_;

    std::mutex mutex_triggered_;
    std::condition_variable condition_triggered_;
    std::atomic<bool> triggered_ = false;

    std::function<void()> trigger_function_;
};

} // namespace wait
} // namespace policy
} // namespace marklar_dataflow
