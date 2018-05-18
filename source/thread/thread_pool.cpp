#include <thread/thread_pool.hpp>

// Marklar headers
#include <error/exception.hpp>
#include <thread/thread_task.hpp>
// 3rd party library headers
// C++ standard Library headers

using marklar_dataflow::error::Exception;
using marklar_dataflow::thread::ThreadPool;
using marklar_dataflow::thread::ThreadRAII;
using marklar_dataflow::thread::ThreadTask;

ThreadPool::ThreadPool()
    : ThreadPool{std::max(std::thread::hardware_concurrency(), 2u) - 1u} /* always create at least one thread */
{}

ThreadPool::ThreadPool(size_t const num_of_threads)
    : running_ {true}
    , queue_ {}
    , threads_ {}
{
    start(num_of_threads);
}

ThreadPool::~ThreadPool()
{
    stop();
}

size_t ThreadPool::thread_number() const
{
    return threads_.size();
}

size_t ThreadPool::queue_size() const
{
    std::unique_lock<std::mutex> lock(mutex_queue_);

    return queue_.size();
}

void ThreadPool::push_front(std::shared_ptr<ThreadTask> task)
{
    task->queued_ = true;

    std::unique_lock<std::mutex> lock(mutex_queue_);

    queue_.emplace_front(std::move(task));

    condition_queue_.notify_one();
}

void ThreadPool::push_back(std::shared_ptr<ThreadTask> task)
{
    task->queued_ = true;

    std::unique_lock<std::mutex> lock(mutex_queue_);

    queue_.emplace_back(std::move(task));

    condition_queue_.notify_one();
}

void ThreadPool::erase(std::shared_ptr<ThreadTask> & task)
{
    std::unique_lock<std::mutex> lock(mutex_queue_);

    auto it = std::find(queue_.begin(), queue_.end(), task);
    if(queue_.cend() != it)
    {
        (*it)->queued_ = false;
        queue_.erase(it);
    }

}

void ThreadPool::start(size_t const num_of_threads)
{
    if(0 == num_of_threads)
        throw Exception("not valid thread number", 0);

    try
    {
        for(size_t i = 0; i < num_of_threads; ++i)
            threads_.emplace_back(std::forward<ThreadRAII>(ThreadRAII(&ThreadPool::run, this)));

        running_ = true;
    }
    catch(std::system_error const & e)
    {
        // TODO
//        throw Exception("exception has occurred while creating threads.(" + e.what() + ")", 0);
        throw Exception(e.what(), 0);
    }
    catch(...)
    {
        stop();

        // TODO
        throw Exception("unknow exception has occurred", 0);
    }
}

void ThreadPool::stop()
{
    if(!running_)
        return;

    running_ = false;
    condition_queue_.notify_all();

    threads_.clear();
//    for(auto& thread : threads_)
//        if(thread.joinable())
//            thread.join();
}

void ThreadPool::run()
{
    while(running_)
    {
        std::shared_ptr<ThreadTask> task = wait_pop();

        if(task)
            task->execute();
    }
}

std::shared_ptr<ThreadTask> ThreadPool::wait_pop()
{
    std::unique_lock<std::mutex> lock(mutex_queue_);

    condition_queue_.wait(
        lock
        , [this]()
        {
            // for stoping
            if(!running_)
                return true;

            return !queue_.empty();
        }
    );

    if(
        queue_.empty()
        || !running_
    )
        return nullptr;

    std::shared_ptr<ThreadTask> task = std::move(queue_.front());
    queue_.pop_front();
    task->queued_ = false;

    return task;
}
