#pragma once

// Marklar headers
#include <attribute.hpp>
// 3rd party library headers
// C++ standard Library headers
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <string>
#include <thread>

// Forward declarations
namespace marklar_dataflow
{
namespace command
{

class Command;
typedef std::shared_ptr<Command> CommandPtr;

class Dashboard;

} // namespace command
} // namespace marklar_dataflow

// Classes and structs
namespace marklar_dataflow
{
namespace command
{

class MARKLAR_EXPORT Controller
{
protected:
    typedef typename std::map<std::string, CommandPtr> CommandMap;
    typedef typename CommandMap::const_iterator const_iterator;

public:
    Controller(Dashboard * node);

    bool is_ready() const;

    void wait() const;

    template< class _RepT, class _PeriodT >
    std::future_status wait_for( std::chrono::duration<_RepT, _PeriodT> const & timeout_duration ) const
    {
        return cmdFuture_.wait_for(timeout_duration);
    }

    void add_command(std::string const & name, CommandPtr const & command);

    Controller & execute(std::string const & name);

    // Element access
    CommandPtr const command(std::string const & name) const; // with bounds checking
    CommandPtr const command(std::string && name) const; // with bounds checking

    // Iterators
    const_iterator begin() const;
    const_iterator cbegin() const;

    const_iterator end() const;
    const_iterator cend() const;

    // Capacity
    bool empty() const;
    size_t size() const;

protected:
    Dashboard * dashboard_; // Not woner, not allowed to delete

    CommandMap commands_;

    std::future<void> cmdFuture_;
};

} // namespace command
} // namespace marklar_dataflow
