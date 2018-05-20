#pragma once

// Marklar headers
#include <attribute.hpp>
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
namespace scheduler
{

class MARKLAR_EXPORT Timed
{
public:
    Timed(std::atomic<bool> & running);
    virtual ~Timed() noexcept = default;

    std::chrono::nanoseconds interval() const;

protected:
    void set_interval(std::chrono::nanoseconds const & interval);

    void preparation();
    void termination();

    bool is_ready();

    std::chrono::nanoseconds interval_ = std::chrono::nanoseconds(0);
    std::chrono::time_point<std::chrono::high_resolution_clock> next_weakup_ = std::chrono::high_resolution_clock::now();

private:
    std::atomic<bool> & running_;

    std::mutex mutex_timed_;
    std::condition_variable condition_timed_;
};

} // namespace scheduler
} // namespace policy
} // namespace marklar_dataflow
