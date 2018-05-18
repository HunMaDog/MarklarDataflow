#include <command/controller.hpp>

// Marklar headers
#include <command/command.hpp>
#include <command/dashboard.hpp>
#include <error/exception.hpp>
// 3rd party library headers
// C++ standard Library headers
#include <utility>

using marklar_dataflow::command::Command;
using marklar_dataflow::command::CommandPtr;
using marklar_dataflow::command::Controller;
using marklar_dataflow::command::Dashboard;
using marklar_dataflow::error::Exception;

Controller::Controller(Dashboard * dashboard)
    : dashboard_(dashboard)
{}

bool Controller::is_ready() const
{
    if(!cmdFuture_.valid())
        return true;

    return cmdFuture_.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

void Controller::wait() const
{
    cmdFuture_.wait();
}

void Controller::add_command(std::string const & name, CommandPtr const & command)
{
    // TODO
    if(!command)
        throw Exception("command is not valid", 0);

    // TODO
    if(commands_.cend() != commands_.find(name))
        throw Exception("command is already exists", 0);

    commands_[name] = command;
}

Controller & Controller::execute(std::string const & name)
{
    auto it = commands_.find(name);
    if(commands_.cend() == it)
        throw Exception("command is not exists", 0);

    if(!is_ready())
        wait();

    auto command = it->second;
    cmdFuture_ = std::async(
        std::launch::async
        , [=](){ command->execute(dashboard_); }
    );

    return *this;
}

// Element access
CommandPtr const Controller::command(std::string const & name) const
{
    auto it = commands_.find(name);
    if(commands_.cend() == it)
        throw Exception("command is not exists", 0);

    return it->second;
}

CommandPtr const Controller::command(std::string && name) const
{
    auto it = commands_.find(std::forward<std::string>(name));
    if(commands_.cend() == it)
        throw Exception("command is not exists", 0);

    return it->second;
}

// Iterators
Controller::const_iterator Controller::begin() const
{
    return commands_.begin();
}

Controller::const_iterator Controller::cbegin() const
{
    return commands_.cbegin();
}

Controller::const_iterator Controller::end() const
{
    return commands_.end();
}

Controller::const_iterator Controller::cend() const
{
    return commands_.cend();
}

// Capacity
bool Controller::empty() const
{
    return commands_.empty();
}

size_t Controller::size() const
{
    return commands_.size();
}
