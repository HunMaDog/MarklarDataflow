#pragma once

// Marklar headers
#include <attribute.hpp>
// 3rd party library headers
// C++ standard Library headers
#include <algorithm>
#include <atomic>
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

class ThreadTask;

} // namespace thread
} // namespace marklar_dataflow

// Classes and structs
namespace marklar_dataflow
{
namespace thread
{

class MARKLAR_EXPORT ThreadRAII
{
public:
    ThreadRAII(std::thread  && thread)
        : thread_(std::forward<std::thread>(thread))
    {}

    template<class _FunctionT, class... _ArgsT>
    ThreadRAII(_FunctionT && f, _ArgsT && ... args)
        : thread_(std::forward<_FunctionT>(f), std::forward<_ArgsT>(args) ...)
    {}

    ThreadRAII(ThreadRAII  && threadraii)
        : thread_(std::forward<std::thread>(threadraii.thread_))
    {}

    ~ThreadRAII()
    {
        // Check if thread is joinable then detach the thread
        if(thread_.joinable())
            thread_.join();
    }

private:
    std::thread thread_;
};

class MARKLAR_EXPORT ThreadPool
{
public:
    ThreadPool();
    explicit ThreadPool(size_t const num_of_threads);

    ThreadPool(ThreadPool const & rhs) = delete; /* non-copyable */
    ThreadPool& operator=(ThreadPool const & rhs) = delete; /* non-assignable */

    ~ThreadPool();

    size_t thread_number() const;
    size_t queue_size() const;

    void push_back(std::shared_ptr<ThreadTask> task);
    void push_front(std::shared_ptr<ThreadTask> task);

    void erase(std::shared_ptr<ThreadTask> & task);

protected:
    void start(size_t const num_of_threads);
    void stop();

    /* each thread run this function */
    void run();

    /* each thread uses to acquire work items from the queue */
    std::shared_ptr<ThreadTask> wait_pop();

private:
    std::atomic<bool> running_;

    mutable std::mutex mutex_queue_;
    std::condition_variable condition_queue_;
    std::deque<std::shared_ptr<ThreadTask>> queue_;

    std::vector<ThreadRAII> threads_;
};

} // namespace thread
} // namespace marklar_dataflow
