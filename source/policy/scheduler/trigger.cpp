#include <policy/scheduler/trigger.hpp>

// Marklar headers
// 3rd party library headers
// C++ standard Library headers

using marklar_dataflow::policy::scheduler::Trigger;

Trigger::Trigger(std::atomic<bool> & running)
    : running_(running)
{}

void Trigger::trigger()
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

void Trigger::set_trigger_function(std::function<void()> && func)
{
    trigger_function_ = std::move(func);
}

void Trigger::preparation()
{}

void Trigger::termination()
{}

bool Trigger::is_ready()
{
    if(triggered_)
    {
        triggered_ = false;
        condition_triggered_.notify_one();

        return running_;
    }

    return false;
}
