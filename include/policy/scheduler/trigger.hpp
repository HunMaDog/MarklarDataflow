#pragma once

// Marklar headers
#include <attribute.hpp>
#include <error/exception.hpp>
// 3rd party library headers
// C++ standard Library headers
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>

namespace marklar_dataflow
{
namespace policy
{
namespace scheduler
{

class MARKLAR_EXPORT Trigger
{
public:
    Trigger(std::atomic<bool> & running);
    virtual ~Trigger() noexcept = default;

    void trigger();

protected:
    void set_trigger_function(std::function<void()> && func);

    void preparation();
    void termination();

    bool is_ready();

private:
    std::atomic<bool> & running_;

    std::mutex mutex_triggered_;
    std::condition_variable condition_triggered_;
    std::atomic<bool> triggered_ = false;

    std::function<void()> trigger_function_;
};

} // namespace scheduler
} // namespace policy
} // namespace marklar_dataflow
