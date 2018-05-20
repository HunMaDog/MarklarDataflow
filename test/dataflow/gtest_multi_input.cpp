#include "gtest/gtest.h"

#include <command/command.hpp>
#include <node/node.hpp>
#include <pin/input_pin.hpp>
#include <pin/output_pin.hpp>
#include <policy/id/uuid.hpp>
#include <policy/wait/timed.hpp>
#include <policy/wait/checklist.hpp>
#include <policy/pin/pin.hpp>

#include <wigwag/thread_task_executor.hpp>

#include <iostream>

using namespace std::chrono_literals;

class Timer
{
public:
    Timer()
        : create_time_(std::chrono::high_resolution_clock::now())
    {}

    void step()
    {
        ++counter_;
    }

    uint64_t step_num()
    {
        return counter_;
    }

    uint64_t lifeTime()
    {
        auto end = std::chrono::high_resolution_clock::now();

        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - create_time_).count();
    }

    uint64_t average()
    {
        auto end = std::chrono::high_resolution_clock::now();

        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - create_time_).count() / counter_;
    }

private:
    uint64_t counter_ = 1;
    std::chrono::high_resolution_clock::time_point create_time_;
};

typedef std::shared_ptr<Timer> TimerPtr;

// Command
using marklar_dataflow::command::Command;
// Policy
using marklar_dataflow::policy::id::Uuid;
using marklar_dataflow::policy::pin::Pin;
using marklar_dataflow::policy::wait::Timed;
using marklar_dataflow::policy::wait::Checklist;
// Node
using marklar_dataflow::command::Dashboard;
using TimedNode = marklar_dataflow::node::Node<Uuid, Timed>;
using TimedOutputNode = marklar_dataflow::node::Node<Uuid, Timed, Pin>;
using IONode = marklar_dataflow::node::Node<Uuid, Checklist, Pin>;
// Pin
using TimerOutputPin = marklar_dataflow::pin::OutputPin<TimerPtr>;
using uint64OutputPin = marklar_dataflow::pin::OutputPin<uint64_t>;
using TimerInputPin = marklar_dataflow::pin::InputPin<TimerPtr>;

class CommandRemovePin
    : public Command
{
protected:
    void command(Dashboard * node) final
    {
        auto pin = dynamic_cast<Pin*>(node);
        pin->remove_pin("output");

//        pin.add_pin(std::make_shared<TimerOutputPin>("output2", condition_run_));
    }
};

class Writer
    : public TimedNode
{
public:
    Writer(std::string const & name, std::chrono::nanoseconds const & interval)
        : TimedNode(name)
    {
        Timed::set_interval(interval);
    }

    void process() final
    {
        std::cout << '.';
    }
};

class Head
    : public TimedOutputNode
{
public:
    Head(std::string const & name, std::chrono::nanoseconds const & interval)
        : TimedOutputNode(name)
    {
        Timed::set_interval(interval);

        add_pin(std::make_shared<TimerOutputPin>("output", [this](){ queuing_task(); }));
    }

    void process() final
    {
        pin<TimerOutputPin>("output")->push(std::make_shared<Timer>());
    }
};

class Inner
    : public IONode
{
public:
    Inner(std::string const & name)
        : IONode(name)
    {
        Checklist::push_check([&](){ return Pin::is_ready(); });

        add_pin(std::make_shared<TimerInputPin>("input", [this](){ queuing_task(); }));
        add_pin(std::make_shared<TimerOutputPin>("output", [this](){ queuing_task(); }));
    }

    void process() final
    {
//        auto data = Input::pin<TimerPtr>("input")->pop();
//        data->step();
        pin<TimerOutputPin>("output")->push(pin<TimerInputPin>("input")->pop_front());
    }
};

class Tail
    : public IONode
{
public:
    Tail(std::string const & name)
        : IONode(name)
    {
        Checklist::push_check([&](){ return Pin::is_ready(); });

        add_pin(std::make_shared<TimerInputPin>("input", [this](){ queuing_task(); }));
        add_pin(std::make_shared<uint64OutputPin>("output", [this](){ queuing_task(); }));
    }

    void process() final
    {
        if(running_)
            pin<uint64OutputPin>("output")->push(pin<TimerInputPin>("input")->pop_front()->lifeTime());
    }
};

template<typename _nodeT>
class NodeRunner
{
public:
    NodeRunner(_nodeT && node)
        : node_(std::forward<_nodeT>(node))
    {
        node_.start();
    }

    ~NodeRunner()
    {
        node_.stop();
    }

private:
    _nodeT node_;
};


namespace
{

TEST(gtest_dataflow_timing, dataflow_row)
{
    using te = wigwag::basic_thread_task_executor<>;
    std::shared_ptr<wigwag::task_executor> executor = std::make_shared<te>();
//    const auto interval = 5ms;
//    const auto interval = 500us;
    const auto interval = 100us;

    auto tp = std::make_shared<marklar_dataflow::thread::ThreadPool>(3);

    auto dotter = std::make_unique<Writer>("writer", 1ms);
//    NodeRunner<Writer> d(std::move(Writer("writer", 1ms)));

    auto head = std::make_unique<Head>("head", interval);

    auto inner1 = std::make_unique<Inner>("inner1");
    auto inner2 = std::make_unique<Inner>("inner2");
    auto inner3 = std::make_unique<Inner>("inner3");

    auto tail = std::make_unique<Tail>("tail");

    dotter->thread_pool(tp);

    inner1->thread_pool(tp);
    inner2->thread_pool(tp);
    inner3->thread_pool(tp);

    tail->thread_pool(tp);

    uint64_t sum = 0;
    uint64_t counter = 0;

    head->pin<TimerOutputPin>("output")->connect(inner1->pin<TimerInputPin>("input"), { executor });
    inner1->pin<TimerOutputPin>("output")->connect(inner2->pin<TimerInputPin>("input"), { executor });
    inner2->pin<TimerOutputPin>("output")->connect(inner3->pin<TimerInputPin>("input"), { executor });
    inner3->pin<TimerOutputPin>("output")->connect(tail->pin<TimerInputPin>("input"), { executor });
    tail->pin<uint64OutputPin>("output")->connect([&sum, &counter](uint64_t number){ ++counter; sum += number; /*std::cout << number << "ns\n";*/ });

    dotter->controller().execute("start").wait();
//    dotter->processing();

    tail->controller().execute("start");
    inner3->controller().execute("start");
    inner2->controller().execute("start");
    inner1->controller().execute("start");
    head->controller().execute("start").wait();

//    head->processing();

    while(counter < 1000)
        std::this_thread::sleep_for(interval);

//    head->stop();

    head->controller().execute("stop").wait();
    inner1->controller().execute("stop").wait();
    inner2->controller().execute("stop").wait();
    inner3->controller().execute("stop").wait();
    tail->controller().execute("stop").wait();

    dotter->controller().execute("stop").wait();


    std::cout << "sum: " << sum << " | data number: " << counter << " | average row: " << (sum / counter) << " | average node: " << (sum / counter / 4) << '\n';

    std::cout << std::flush;
}

} // namespace
