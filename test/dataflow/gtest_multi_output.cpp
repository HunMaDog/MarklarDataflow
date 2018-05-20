#include "gtest/gtest.h"

#include <command/command.hpp>
#include <node/node.hpp>
#include <pin/input_pin.hpp>
#include <pin/output_pin.hpp>
#include <policy/id/name.hpp>
#include <policy/id/uuid.hpp>
#include <policy/wait/checklist.hpp>
#include <policy/wait/timed.hpp>
#include <policy/wait/trigger.hpp>
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
namespace cmd = marklar_dataflow::command;
// Policy
using marklar_dataflow::policy::id::Name;
using marklar_dataflow::policy::id::Uuid;
using marklar_dataflow::policy::pin::Pin;
using marklar_dataflow::policy::wait::Checklist;
using marklar_dataflow::policy::wait::Timed;
using marklar_dataflow::policy::wait::Trigger;
// Node
using marklar_dataflow::command::Dashboard;
using TimedNode = marklar_dataflow::node::Node<Uuid, Timed>;
using TimedOutputNode = marklar_dataflow::node::Node<Uuid, Timed, Pin>;
using IONode = marklar_dataflow::node::Node<Uuid, Checklist, Pin>;
using TriggerNode = marklar_dataflow::node::Node<Name, Trigger>;
// Pin
using TimerOutputPin = marklar_dataflow::pin::OutputPin<TimerPtr>;
using uint64OutputPin = marklar_dataflow::pin::OutputPin<uint64_t>;
using boolOutputPin = marklar_dataflow::pin::OutputPin<bool>;
using TimerInputPin = marklar_dataflow::pin::InputPin<TimerPtr>;
using uint64InputPin = marklar_dataflow::pin::InputPin<uint64_t>;
using boolInputPin = marklar_dataflow::pin::InputPin<bool>;

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
        pin<uint64OutputPin>("output")->push(pin<TimerInputPin>("input")->pop_front()->lifeTime());
    }
};

class Sequencer
    : public TimedOutputNode
{
public:
    Sequencer(std::string const & name, std::chrono::nanoseconds const & interval)
        : TimedOutputNode(name)
    {
        Timed::set_interval(interval);

        add_pin(std::make_shared<uint64OutputPin>("output", [this](){ queuing_task(); }));
    }

    void process() final
    {
        pin<uint64OutputPin>("output")->push((++counter_));
    }

protected:
    uint64_t counter_ = 0;
};

class SequenceChecker
    : public IONode
{
public:
    SequenceChecker(std::string const & name)
        : IONode(name)
    {
        Checklist::push_check([&](){ return Pin::is_ready(); });

        add_pin(std::make_shared<uint64InputPin>("input", [this](){ queuing_task(); }));
        add_pin(std::make_shared<boolOutputPin>("output", [this](){ queuing_task(); }));
    }

    void process() final
    {
        uint64_t nextCounter = pin<uint64InputPin>("input")->pop_front();

        bool is_sequence = false;
        if((counter_ + 1) == nextCounter)
        {
            is_sequence = true;
        }
        else
        {
            std::cout << '('<< counter_ << '!' << nextCounter << ")\n";
        }
        counter_ = nextCounter;

        pin<boolOutputPin>("output")->push(is_sequence);
    }

protected:
    uint64_t counter_ = 0;
};

#include <wigwag/signal.hpp>
class Commander
{
public:
    void command(std::string const & cmd)
    {
        command_signal_(cmd);
    }

    template<class _NodeT>
    void connect(std::shared_ptr<_NodeT> const & node)
    {
        tokens_.emplace_back(
            command_signal_.connect([node](std::string const & command){ node->controller().execute(command); })
        );
    }

protected:
    wigwag::signal<void(std::string const &)> command_signal_;

    std::vector<wigwag::token> tokens_;
};

class HelloWorld
    : public TriggerNode
{
public:
    HelloWorld(std::string const & name)
        : TriggerNode(name)
    {
        set_trigger_function([this](){ queuing_task(); });
    }

protected:
    void process() final
    {
        std::cout << "Hello world" << std::endl;
    }
};

namespace
{

TEST(gtest_dataflow_output, multi_output)
{
    using te = wigwag::basic_thread_task_executor<>;
    std::shared_ptr<wigwag::task_executor> executor = std::make_shared<te>();
//    const auto interval = 500us;
    const auto interval = 100us;

    auto head = std::make_shared<Head>("headx", interval);
    auto tail1 = std::make_shared<Tail>("tail1");
    auto tail2 = std::make_shared<Tail>("tail2");
    auto tail3 = std::make_shared<Tail>("tail3");
    auto tail4 = std::make_shared<Tail>("tail4");
    auto tail5 = std::make_shared<Tail>("tail5");
    auto tail6 = std::make_shared<Tail>("tail6");
    auto tail7 = std::make_shared<Tail>("tail7");

    uint64_t sum1 = 0;
    uint64_t counter1 = 0;
    uint64_t sum2 = 0;
    uint64_t counter2 = 0;
    uint64_t sum3 = 0;
    uint64_t counter3 = 0;
    uint64_t sum4 = 0;
    uint64_t counter4 = 0;
    uint64_t sum5 = 0;
    uint64_t counter5 = 0;
    uint64_t sum6 = 0;
    uint64_t counter6 = 0;
    uint64_t sum7 = 0;
    uint64_t counter7 = 0;

/*
    head->pin<TimerOutputPin>("output")->connect(tail1->pin<TimerInputPin>("input"));
    head->pin<TimerOutputPin>("output")->connect(tail2->pin<TimerInputPin>("input"));
    head->pin<TimerOutputPin>("output")->connect(tail3->pin<TimerInputPin>("input"));
    head->pin<TimerOutputPin>("output")->connect(tail4->pin<TimerInputPin>("input"));
    head->pin<TimerOutputPin>("output")->connect(tail5->pin<TimerInputPin>("input"));
    head->pin<TimerOutputPin>("output")->connect(tail6->pin<TimerInputPin>("input"));
    head->pin<TimerOutputPin>("output")->connect(tail7->pin<TimerInputPin>("input"));
*/

    head->pin<TimerOutputPin>("output")->connect(tail1->pin<TimerInputPin>("input"), { executor });
    head->pin<TimerOutputPin>("output")->connect(tail2->pin<TimerInputPin>("input"), { executor });
    head->pin<TimerOutputPin>("output")->connect(tail3->pin<TimerInputPin>("input"), { executor });
    head->pin<TimerOutputPin>("output")->connect(tail4->pin<TimerInputPin>("input"), { executor });
    head->pin<TimerOutputPin>("output")->connect(tail5->pin<TimerInputPin>("input"), { executor });
    head->pin<TimerOutputPin>("output")->connect(tail6->pin<TimerInputPin>("input"), { executor });
    head->pin<TimerOutputPin>("output")->connect(tail7->pin<TimerInputPin>("input"), { executor });


    tail1->pin<uint64OutputPin>("output")->connect([&sum1, &counter1](uint64_t number){ ++counter1; sum1 += number; });
    tail2->pin<uint64OutputPin>("output")->connect([&sum2, &counter2](uint64_t number){ ++counter2; sum2 += number; });
    tail3->pin<uint64OutputPin>("output")->connect([&sum3, &counter3](uint64_t number){ ++counter3; sum3 += number; });
    tail4->pin<uint64OutputPin>("output")->connect([&sum4, &counter4](uint64_t number){ ++counter4; sum4 += number; });
    tail5->pin<uint64OutputPin>("output")->connect([&sum5, &counter5](uint64_t number){ ++counter5; sum5 += number; });
    tail6->pin<uint64OutputPin>("output")->connect([&sum6, &counter6](uint64_t number){ ++counter6; sum6 += number; });
    tail7->pin<uint64OutputPin>("output")->connect([&sum7, &counter7](uint64_t number){ ++counter7; sum7 += number; });

    auto tp = std::make_shared<marklar_dataflow::thread::ThreadPool>();

    tail1->thread_pool(tp);
    tail2->thread_pool(tp);
    tail3->thread_pool(tp);
    tail4->thread_pool(tp);
    tail5->thread_pool(tp);
    tail6->thread_pool(tp);
    tail7->thread_pool(tp);

    Commander commander;
    commander.connect(head);
    commander.connect(tail1);
    commander.connect(tail2);
    commander.connect(tail3);
    commander.connect(tail4);
    commander.connect(tail5);
    commander.connect(tail6);
    commander.connect(tail7);

    commander.command(cmd::start);

    head->controller().execute("start").wait();
//    head->queuing_task();

    while(
        counter1 < 1000
        || counter2 < 1000
        || counter3 < 1000
        || counter4 < 1000
        || counter5 < 1000
        || counter6 < 1000
        || counter7 < 1000
    )
        std::this_thread::sleep_for(interval);

    commander.command(cmd::stop);

    std::cout << "tail1 sum: " << sum1 << " | data number: " << counter1 << " | average: " << (sum1 / counter1) << '\n';
    std::cout << "tail2 sum: " << sum2 << " | data number: " << counter2 << " | average: " << (sum2 / counter2) << '\n';
    std::cout << "tail3 sum: " << sum3 << " | data number: " << counter3 << " | average: " << (sum3 / counter3) << '\n';
    std::cout << "tail4 sum: " << sum4 << " | data number: " << counter4 << " | average: " << (sum4 / counter4) << '\n';
    std::cout << "tail5 sum: " << sum5 << " | data number: " << counter5 << " | average: " << (sum5 / counter5) << '\n';
    std::cout << "tail6 sum: " << sum6 << " | data number: " << counter6 << " | average: " << (sum6 / counter6) << '\n';
    std::cout << "tail7 sum: " << sum7 << " | data number: " << counter7 << " | average: " << (sum7 / counter7) << '\n';

    std::cout << std::flush;
}

TEST(gtest_dataflow_output, multi_input)
{
    using te = wigwag::basic_thread_task_executor<>;
    std::shared_ptr<wigwag::task_executor> executor = std::make_shared<te>();
    const auto interval = 100us;

    auto head1 = std::make_shared<Head>("head1", interval);
    auto head2 = std::make_shared<Head>("head2", interval);
    auto head3 = std::make_shared<Head>("head3", interval);
    auto head4 = std::make_shared<Head>("head4", interval);
    auto head5 = std::make_shared<Head>("head5", interval);
    auto head6 = std::make_shared<Head>("head6", interval);
    auto head7 = std::make_shared<Head>("head7", interval);

    auto tail = std::make_shared<Tail>("tailx");

    uint64_t sum = 0;
    uint64_t counter = 0;

    head1->pin<TimerOutputPin>("output")->connect(tail->pin<TimerInputPin>("input"), { executor });
    head2->pin<TimerOutputPin>("output")->connect(tail->pin<TimerInputPin>("input"), { executor });
    head3->pin<TimerOutputPin>("output")->connect(tail->pin<TimerInputPin>("input"), { executor });
    head4->pin<TimerOutputPin>("output")->connect(tail->pin<TimerInputPin>("input"), { executor });
    head5->pin<TimerOutputPin>("output")->connect(tail->pin<TimerInputPin>("input"), { executor });
    head6->pin<TimerOutputPin>("output")->connect(tail->pin<TimerInputPin>("input"), { executor });
    head7->pin<TimerOutputPin>("output")->connect(tail->pin<TimerInputPin>("input"), { executor });
    tail->pin<uint64OutputPin>("output")->connect([&sum, &counter](uint64_t number){ ++counter; sum += number; });

    Commander commander;
    commander.connect(head1);
    commander.connect(head2);
    commander.connect(head3);
    commander.connect(head4);
    commander.connect(head5);
    commander.connect(head6);
    commander.connect(head7);
    commander.connect(tail);

    commander.command(cmd::start);

    tail->controller().execute("start").wait();

    head1->controller().execute("start").wait();
//    head1->queuing_task();

    head2->controller().execute("start").wait();
//    head2->processing();

    head3->controller().execute("start").wait();
//    head3->processing();

    head4->controller().execute("start").wait();
//    head4->processing();

    head5->controller().execute("start").wait();
//    head5->processing();

    head6->controller().execute("start").wait();
//    head6->processing();

    head7->controller().execute("start").wait();
//    head7->processing();

    while(
        counter < 1000
    )
        std::this_thread::sleep_for(interval);

//    commander.command(cmd::stop);
    head1->controller().execute("stop").wait();
    head2->controller().execute("stop").wait();
    head3->controller().execute("stop").wait();
    head4->controller().execute("stop").wait();
    head5->controller().execute("stop").wait();
    head6->controller().execute("stop").wait();
    head7->controller().execute("stop").wait();
    tail->controller().execute("stop").wait();

    std::cout << "tail sum: " << sum << " | data number: " << counter << " | average: " << (sum / counter) << '\n';

    std::cout << std::flush;
}

TEST(gtest_dataflow_output, sequencing)
{
    using te = wigwag::basic_thread_task_executor<>;
    std::shared_ptr<wigwag::task_executor> executor = std::make_shared<te>();
    const auto interval = 500us;

//    auto console = spdlog::stdout_color_mt("console");

    auto seq = std::make_shared<Sequencer>("sequencer", interval);
    auto seqCheck = std::make_shared<SequenceChecker>("sequence_checker");
    bool is_good = true;

    seq->pin<uint64OutputPin>("output")->connect(seqCheck->pin<uint64InputPin>("input"), { executor });
    seqCheck->pin<boolOutputPin>("output")->connect([&is_good](bool const & is_sequence){ if(!is_sequence) is_good = false; });

    Commander commander;
    commander.connect(seq);
    commander.connect(seqCheck);

    commander.command(cmd::start);

    seq->controller().execute("start").wait();
//    seq->processing();

    std::this_thread::sleep_for(interval * 1000);

//    auto begin = std::chrono::high_resolution_clock::now();
//    seqCheck->controller().execute("stop");
//    auto end = std::chrono::high_resolution_clock::now();
//    std::cout << "cmd time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() << '\n';

//    commander.command(cmd::stop);
    seq->controller().execute("stop").wait();
    seqCheck->controller().execute("stop").wait();

    {
        auto hi = std::make_shared<HelloWorld>("hello");
        hi->start();
        hi->trigger();
        hi->stop();
    }

    std::cout << "Thread: " << std::thread::hardware_concurrency() << " : " << (std::max(std::thread::hardware_concurrency(), 2u) - 1u) << std::endl;

    EXPECT_TRUE(is_good);
}

} // namespace
