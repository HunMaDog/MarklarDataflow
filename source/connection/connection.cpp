#include <connection/connection.hpp>

// Marklar headers
#include <connection/connection_pool.hpp>
// 3rd party library headers
// C++ standard Library headers

using marklar_dataflow::connection::Connection;
using marklar_dataflow::connection::ConnectionPool;

Connection::Connection(wigwag::token && connection_token)
    : uuid_(sole::uuid4())
    , token_(std::move(connection_token))
{}

sole::uuid const & Connection::id() const
{
    return uuid_;
}

std::weak_ptr<ConnectionPool> const Connection::emitter_pool() const
{
    return emitter_pool_;
}

std::weak_ptr<ConnectionPool> const Connection::receiver_pool() const
{
    return receiver_pool_;
}

void Connection::disconnect() const
{
    std::scoped_lock lock(mutex_disconnect_);

    auto emitter_pool = emitter_pool_.lock();
    if(emitter_pool)
        emitter_pool->erase(uuid_);

    auto receiver_pool = receiver_pool_.lock();
    if(receiver_pool)
        receiver_pool->erase(uuid_);
}
