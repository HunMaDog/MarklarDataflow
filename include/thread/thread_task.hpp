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
#include <shared_mutex>
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

class MARKLAR_EXPORT BaseTask
{
private:
    friend class ThreadPool;

public:
    BaseTask()
        : queued_(false)
        , progress_(false)
    {}

    ~BaseTask() = default;
    BaseTask(BaseTask const & rhs) = delete;
    BaseTask& operator=(BaseTask const & rhs) = delete;
    BaseTask(BaseTask && other) = default;
    BaseTask& operator=(BaseTask && other) = default;

    bool is_queued() const
    {
        return queued_;
    }

    bool is_progress() const
    {
        return progress_;
    }

    bool is_avaliable() const
    {
        return !(progress_ || queued_);
    }

protected:
    std::atomic<bool> queued_;
    std::atomic<bool> progress_;

private:
    virtual void execute() = 0;
};

template<class _TaskOwnerT>
class MARKLAR_EXPORT NodeTask final
    : public BaseTask
{
public:
    typedef void (_TaskOwnerT::*_TaskFunctionT)(void);

    NodeTask(_TaskOwnerT * task_owner, _TaskFunctionT task_function)
        : BaseTask()
        , task_owner_(task_owner)
        , task_function_(task_function)
    {
    }

    ~NodeTask() = default;
    NodeTask(NodeTask const & rhs) = delete;
    NodeTask& operator=(NodeTask const & rhs) = delete;
    NodeTask(NodeTask && other) = default;
    NodeTask& operator=(NodeTask && other) = default;

    void wait()
    {
        if(is_avaliable())
            return;

        std::shared_lock<std::shared_mutex> lock(mutex_wait_);

        condition_wait_.wait(
            lock
            , [this]()
            {
                return is_avaliable();
            }
        );
    }

    template< class _RepT, class _PeriodT >
    std::cv_status wait_for( std::chrono::duration<_RepT, _PeriodT> const & timeout_duration )
    {
        if(is_avaliable())
            return std::cv_status::no_timeout;

        std::shared_lock<std::shared_mutex> lock(mutex_wait_);

        return condition_wait_.wait_for(lock, timeout_duration);
    }

private:
    void execute() final
    {
        std::unique_lock<std::shared_mutex> lock(mutex_wait_);

        progress_ = true;
        (task_owner_->*task_function_)();
        progress_ = false;

        condition_wait_.notify_all();
    }

    _TaskOwnerT * task_owner_;
    _TaskFunctionT task_function_;

    mutable std::shared_mutex mutex_wait_;
    std::condition_variable_any condition_wait_;
};

} // namespace thread
} // namespace marklar_dataflow
