#include <pin/pin.hpp>

// Marklar headers
#include <connection/connection_pool.hpp>
// 3rd party library headers
#include <spdlog/spdlog.h>
// C++ standard Library headers
#include <type_traits>

using marklar_dataflow::connection::ConnectionPool;
using marklar_dataflow::connection::ConnectionPoolPtr;
using marklar_dataflow::LoggerPtr;
using marklar_dataflow::pin::Pin;

Pin::Pin(
    std::string const & name
    , Flag const & flag
)
    : name_(name)
    , flag_(flag)
    , logger_(nullptr)
    , connection_pool_(std::make_shared<ConnectionPool>())
{}

std::string const & Pin::name() const
{
    return name_;
}

Pin::Flag Pin::flag() const
{
    return flag_;
}

LoggerPtr Pin::logger()
{
    return logger_;
}

LoggerPtr const Pin::logger() const
{
    return logger_;
}

ConnectionPoolPtr const Pin::connection_pool() const
{
    return connection_pool_;
}

std::string & Pin::name()
{
    return name_;
}

void Pin::set_logger(LoggerPtr const & logger)
{
    logger_ = logger;
}

bool Pin::connectable()
{
    if(Flag::Locked == (flag_ & Flag::Locked))
        return false;

    if(
        Flag::Single == (flag_  & Flag::Single)
        && connection_pool_->size() != 0
    )
        return false;

    return true;
}

// Flag bitmask operators
using FlagT = std::underlying_type_t<Pin::Flag>;

Pin::Flag operator | (Pin::Flag const & lhs, Pin::Flag const & rhs)
{
    return static_cast<Pin::Flag>(static_cast<FlagT>(lhs) | static_cast<FlagT>(rhs));
}

Pin::Flag & operator |= (Pin::Flag & lhs, Pin::Flag const & rhs)
{
    lhs = static_cast<Pin::Flag>(static_cast<FlagT>(lhs) | static_cast<FlagT>(rhs));

    return lhs;
}

Pin::Flag operator & (Pin::Flag const & lhs, Pin::Flag const & rhs)
{
    return static_cast<Pin::Flag>(static_cast<FlagT>(lhs) & static_cast<FlagT>(rhs));
}

Pin::Flag & operator &= (Pin::Flag & lhs, Pin::Flag const & rhs)
{
    lhs = static_cast<Pin::Flag>(static_cast<FlagT>(lhs) & static_cast<FlagT>(rhs));

    return lhs;
}

Pin::Flag operator ^ (Pin::Flag const & lhs, Pin::Flag const & rhs)
{
    return static_cast<Pin::Flag>(static_cast<FlagT>(lhs) ^ static_cast<FlagT>(rhs));
}

Pin::Flag & operator ^= (Pin::Flag & lhs, Pin::Flag const & rhs)
{
    lhs = static_cast<Pin::Flag>(static_cast<FlagT>(lhs) ^ static_cast<FlagT>(rhs));

    return lhs;
}
