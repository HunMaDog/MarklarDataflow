#include <command/command.hpp>

// Marklar headers
#include <command/dashboard.hpp>
#include <connection/connection_pool.hpp>
#include <error/exception.hpp>
// 3rd party library headers
// C++ standard Library headers

using marklar_dataflow::command::Command;
using marklar_dataflow::command::Dashboard;
using marklar_dataflow::connection::ConnectionPoolPtr;
using marklar_dataflow::data::Properties;
using marklar_dataflow::error::Exception;

void Command::execute(Dashboard * dashboard)
{
    Result result = Result::None;

    try
    {
        preparation_properties();
        command(dashboard);
        termination_properties();

        result = Result::Success;
    }
    catch(Exception const & exception)
    {
        result = Result::Exception;
    }
    catch(...)
    {
        result = Result::Exception;
    }

    ready_signal_(result);
}

Properties & Command::properties()
{
    return properties_;
}

Properties const & Command::properties() const
{
    return properties_;
}

ConnectionPoolPtr const Command::connection_pool() const
{
    return connection_pool_;
}

sole::uuid const & Command::connect(
    std::function<void(Result const &)> const & func
    , connection::TokenProperties const & properties
)
{
    return connect_func(func, properties);
}

void Command::operator+=(std::function<void(Result const &)> const & func)
{
    connect_func(func);
}

sole::uuid const & Command::connect_func(
    std::function<void(Result const &)> const & func
    , connection::TokenProperties const & properties
)
{
    wigwag::token token;
    if(properties.worker_)
        token = ready_signal_.connect(properties.worker_, func, properties.handler_);
    else
        token = ready_signal_.connect(func, properties.handler_);

    auto new_connection = std::make_shared<connection::Connection>(std::forward<wigwag::token>(token));
    connection::save(new_connection, { connection_pool_, nullptr } );

    return new_connection->id();
}
