#pragma once

// Marklar headers
#include <attribute.hpp>
// 3rd party library headers
// C++ standard Library headers
#include <atomic>
#include <memory>
#include <string>
#include <typeindex>

// Forward declarations
namespace spdlog
{

class logger;

} // namespace spdlog

namespace marklar_dataflow
{

typedef std::shared_ptr<spdlog::logger> LoggerPtr;

namespace connection
{

class ConnectionPool;
typedef std::shared_ptr<ConnectionPool> ConnectionPoolPtr;

} // namespace connection
} // namespace marklar_dataflow

// Classes and structs
namespace marklar_dataflow
{
namespace pin
{

class MARKLAR_EXPORT Pin
{
public:
    enum class Flag : int
    {
        None = 0
        , Accept = 1 // Accept arriving data
        , Publish = 2 // Publishing arrived data
        , Fixed = 4 // Allways publishing the last arrived data and only one data has
        , Optional = 8 // Not required for node's valid operation
        , Silent = 16 // Not use wake up signal
        , Single = 32 // Maximum 1 connection is allowed
        , Locked = 64 // Not accept any new connection
// TODO :: design it
//        , Wait = 128 // Wait until the buffer has free space
    };

    Pin(
        std::string const & name
        , Flag const & flag = Flag::None
    );
    virtual ~Pin() noexcept = default;

    // Element access
    std::string const & name() const;

    Flag flag() const;

    virtual std::type_index data_type() const = 0;

    LoggerPtr logger();
    LoggerPtr const logger() const;

    connection::ConnectionPoolPtr const connection_pool() const;

    // Modifiers
    std::string & name();

    virtual void set_flag(Flag const & flag) = 0;
    virtual void unset_flag(Flag const & flag) = 0;

    void set_logger(LoggerPtr const & logger);

    // Checks
    bool connectable();

    virtual bool is_ready() const = 0;

protected:
    // Members
    std::string name_;

    std::atomic<Flag> flag_;

    LoggerPtr logger_;

    connection::ConnectionPoolPtr connection_pool_;
};

} // namespace pin
} // namespace marklar_dataflow

// Flag bitmask operators
marklar_dataflow::pin::Pin::Flag operator | (marklar_dataflow::pin::Pin::Flag const & lhs, marklar_dataflow::pin::Pin::Flag const & rhs);
marklar_dataflow::pin::Pin::Flag & operator |= (marklar_dataflow::pin::Pin::Flag & lhs, marklar_dataflow::pin::Pin::Flag const & rhs);
marklar_dataflow::pin::Pin::Flag operator & (marklar_dataflow::pin::Pin::Flag const & lhs, marklar_dataflow::pin::Pin::Flag const & rhs);
marklar_dataflow::pin::Pin::Flag & operator &= (marklar_dataflow::pin::Pin::Flag & lhs, marklar_dataflow::pin::Pin::Flag const & rhs);
marklar_dataflow::pin::Pin::Flag operator ^ (marklar_dataflow::pin::Pin::Flag const & lhs, marklar_dataflow::pin::Pin::Flag const & rhs);
marklar_dataflow::pin::Pin::Flag & operator ^= (marklar_dataflow::pin::Pin::Flag & lhs, marklar_dataflow::pin::Pin::Flag const & rhs);
