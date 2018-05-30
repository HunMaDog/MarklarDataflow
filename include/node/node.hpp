#pragma once

// Marklar headers
#include <attribute.hpp>
#include <command/command_start.hpp>
#include <command/command_stop.hpp>
#include <command/controller.hpp>
#include <command/dashboard.hpp>
#include <data/properties.hpp>
#include <error/exception.hpp>
#include <thread/thread_pool.hpp>
#include <thread/thread_task.hpp>
// 3rd party library headers
#include <spdlog/spdlog.h>
// C++ standard Library headers
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>
#include <string>
#include <thread>

// Forward declarations
// Classes and structs
namespace marklar_dataflow
{

typedef std::shared_ptr<spdlog::logger> LoggerPtr;

namespace node
{

template<class _PolicyIdentifierT, class _PolicySchedulerT, class... _PolicyExtenderT>
class MARKLAR_EXPORT Node
        : public command::Dashboard // For controling node
        , public _PolicyIdentifierT // For storage in containers
        , public _PolicySchedulerT // For trigger the main loop
        , public _PolicyExtenderT... // Pin and other behavior/additional policy
{
private:
    typedef Node<_PolicyIdentifierT, _PolicySchedulerT, _PolicyExtenderT...> ThisNode;

public:
    Node(std::string const & name)
        : Dashboard()
        , _PolicyIdentifierT(name)
        , _PolicySchedulerT(running_)
        , _PolicyExtenderT()...
        , running_(false)
        , controller_(this)
        , thread_task_(nullptr)
        , thread_pool_(nullptr)
        , logger_(nullptr)
    {
        controller_.add_command(command::start, std::make_shared<command::Start>());
        controller_.add_command(command::stop, std::make_shared<command::Stop>());
    }

    virtual ~Node() noexcept
    {
        if(running_)
            stop();
    }

    data::Properties & properties()
    {
        return properties_;
    }

    data::Properties const & properties() const
    {
        return properties_;
    }

    command::Controller & controller()
    {
        return controller_;
    }

    command::Controller const & controller() const
    {
        return controller_;
    }

    bool is_running() const
    {
        return running_;
    }

    void thread_pool(std::shared_ptr<thread::ThreadPool> const & thread_pool)
    {
        thread_pool_ = thread_pool;
    }

    LoggerPtr logger()
    {
        return logger_;
    }

    LoggerPtr const logger() const
    {
        return logger_;
    }

    void set_logger(LoggerPtr const & logger)
    {
        logger_ = logger;
    }

protected:
    /* Calling before start the node inner cycle */
    virtual void preparation()
    {}

    /* Calling after finished the node inner cycle */
    virtual void termination()
    {}

    virtual void passthrough_impl() override
    {
        // TODO
        throw error::Exception("Not implemented", 0);
    }

    virtual void pause_impl() override
    {
        // TODO
        throw error::Exception("Not implemented", 0);
    }

    virtual void start_impl() override
    {
        if(running_)
            return;

        /* Wait policy preparation for waiting */
        _PolicySchedulerT::preparation();
        /* Initialize task and thread pool */
        preparation_thread();
        /* Node preaparation for running the core */
        preparation();

        running_ = true;

        queuing_task();
    }

    virtual void stop_impl() override
    {
        if (running_)
        {
            running_ = false;

            /* Stop thread handling and wait the running task finishing up */
            termination_thread();
            /* Wait policy postprocess after finishing the runing */
            _PolicySchedulerT::termination();
            /* Node postprocess after finishing the runing */
            termination();
        }
    }

    void queuing_task()
    {
        if (!running_)
            return;

        if(
            !thread_pool_
            || !thread_task_
        )
            return;

        if(!thread_task_->is_avaliable())
            return;

        thread_pool_->push_back(thread_task_);
    }

    void preparation_thread()
    {
        if(!thread_task_)
            thread_task_ = std::make_shared<thread::NodeTask<ThisNode>>(this, &ThisNode::task);

        if(!thread_pool_)
            thread_pool_ = std::make_shared<thread::ThreadPool>(1);
    }

    void termination_thread()
    {
        if(thread_pool_ && thread_task_)
            thread_pool_->erase(thread_task_);

        if(thread_task_)
            if(!thread_task_->is_avaliable())
                thread_task_->wait();
    }

    /* Flag what is indicate the run state */
    std::atomic<bool> running_;

    /* Storage information for running */
    data::Properties properties_;

    /* Async controller using the public function */
    command::Controller controller_;

    /* Thread task and pool what can run the task */
    std::shared_ptr<thread::NodeTask<ThisNode>> thread_task_;
    std::shared_ptr<thread::ThreadPool> thread_pool_;

    LoggerPtr logger_;

private:
    virtual void process() = 0;

    void task()
    {
        try
        {
            while(
                running_
                && _PolicySchedulerT::is_ready()
            )
                process();
        }
        catch (const error::Exception& e)
        {
            std::cout << "error in run: " << e.what() << std::endl;
//              LOG(Error, MakeString() << name_ << ": error while processing: " << e.what());
        }
        catch (...)
        {
            std::cout << "other error in run" << std::endl;
            // TODO :: rethink, maybe need stop everything?
//              LOG(Error, MakeString() << name_ << ": unknown error while processing");
        }
    }
};

} // namespace node
} // namespace marklar_dataflow
