#include <thread/thread_task.hpp>

// Marklar headers
#include <thread/thread_pool.hpp>
// 3rd party library headers
// C++ standard Library headers

using marklar_dataflow::thread::ThreadTask;

ThreadTask::ThreadTask(std::function<void()> && func)
    : queued_(false)
    , progress_(false)
    , func_{std::move(func)}
{
}

void ThreadTask::wait()
{
    std::unique_lock<std::mutex> lock(mutex_wait_);

    condition_wait_.wait(
        lock
        , [this]()
        {
            return is_avaliable();
        }
    );
}

bool ThreadTask::is_queued() const
{
    return queued_;
}

bool ThreadTask::is_progress() const
{
    return progress_;
}

bool ThreadTask::is_avaliable() const
{
    return !(progress_ || queued_);
}

void ThreadTask::execute()
{
    progress_ = true;
    func_();
    progress_ = false;

    condition_wait_.notify_one();
}

