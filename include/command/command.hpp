#pragma once

// Marklar headers
#include <attribute.hpp>
#include <connection/connection.hpp>
#include <data/properties.hpp>
// 3rd party library headers
//#include <uuid.h>
#include <sole.fwd.hpp>
#include <wigwag/signal.hpp>
// C++ standard Library headers
#include <functional>

// Forward declarations
namespace marklar_dataflow
{
namespace command
{
    class Dashboard;
} // namespace command

namespace connection
{
    class ConnectionPool;
    typedef std::shared_ptr<ConnectionPool> ConnectionPoolPtr;
} // namespace connection
} // namespace marklar_dataflow

// Classes and structs
namespace marklar_dataflow
{
namespace command
{

// TODO :: error returning
class MARKLAR_EXPORT Command
{
public:
    enum class Result : int
    {
        None = 0
        , Success = 1
        , Exception = 2
    };

    virtual ~Command() noexcept = default;

    void execute(Dashboard * dashboard);

    data::Properties & properties();

    data::Properties const & properties() const;

    connection::ConnectionPoolPtr const connection_pool() const;

    // Connectors
    sole::uuid const & connect(
        std::function<void(Result const &)> const & func
        , connection::TokenProperties const & properties = {}
    );

    void operator+=(std::function<void(Result const &)> const & func);

protected:
    sole::uuid const & connect_func(
        std::function<void(Result const &)> const & func
        , connection::TokenProperties const & properties = {}
    );

    virtual void preparation_properties() {};
    virtual void termination_properties() {};
    virtual void command(Dashboard * dashboard) = 0;

    /* Storage information for command executing */
    data::Properties properties_;

private:
    connection::ConnectionPoolPtr connection_pool_;

    wigwag::signal<void(Result const &)> ready_signal_;
};

} // namespace command
} // namespace marklar_dataflow
