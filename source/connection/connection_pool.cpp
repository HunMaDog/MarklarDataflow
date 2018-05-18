#include <connection/connection_pool.hpp>

// Marklar headers
#include <error/exception.hpp>
// 3rd party library headers
#include <sole.hpp>
// C++ standard Library headers
#include <mutex>
#include <utility>

using marklar_dataflow::connection::Connection;
using marklar_dataflow::connection::ConnectionPtr;
using marklar_dataflow::connection::ConnectionPool;
using marklar_dataflow::connection::ConnectionPoolPtr;
using marklar_dataflow::error::Exception;

ConnectionPool::~ConnectionPool()
{
    clear();
}

// Element access
ConnectionPtr const ConnectionPool::connection(sole::uuid const & id) const
{
    try
    {
        std::shared_lock<std::shared_mutex> lock(mutex_connections_);

        return connections_.at(id);
    }
    catch(std::out_of_range const & e)
    {
        // TODO
        throw Exception("connection is not found",0);
    }
}

ConnectionPtr const ConnectionPool::connection(sole::uuid && id) const
{
    try
    {
        std::shared_lock<std::shared_mutex> lock(mutex_connections_);

        return connections_.at(std::forward<sole::uuid>(id));
    }
    catch(std::out_of_range const & e)
    {
        // TODO
        throw Exception("connection is not found",0);
    }
}

// Iterators
ConnectionPool::const_iterator ConnectionPool::begin() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_connections_);

    return connections_.begin();
}

ConnectionPool::const_iterator ConnectionPool::cbegin() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_connections_);

    return connections_.cbegin();
}

ConnectionPool::const_iterator ConnectionPool::end() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_connections_);

    return connections_.end();
}

ConnectionPool::const_iterator ConnectionPool::cend() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_connections_);

    return connections_.cend();
}

// Capacity
bool ConnectionPool::empty() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_connections_);

    return connections_.empty();
}

size_t ConnectionPool::size() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_connections_);

    return connections_.size();
}

// Modifiers
void ConnectionPool::clear()
{
    while(!connections_.empty())
    {
        ConnectionPtr connection;

        {
            std::unique_lock<std::shared_mutex> lock(mutex_connections_);

            auto it = connections_.cbegin();
            if(connections_.cend() != it)
            {
                connection = std::move(it->second);
                it = connections_.erase(it);
            }
        }

        if(connection)
        {
            connection->disconnect();
        }
    }
}

void ConnectionPool::insert(ConnectionPtr const & connection)
{
    std::unique_lock<std::shared_mutex> lock(mutex_connections_);

    auto id = connection->id();

    // TODO
    if(connections_.cend() != connections_.find(id))
        throw Exception("already existing",0);

    connections_[id] = connection;
}

void ConnectionPool::insert(ConnectionPtr && connection)
{
    std::unique_lock<std::shared_mutex> lock(mutex_connections_);

    auto id = connection->id();

    // TODO
    if(connections_.cend() != connections_.find(id))
        throw Exception("already existing",0);

    connections_[id] = std::forward<ConnectionPtr>(connection);
}

void ConnectionPool::erase(sole::uuid const & id)
{
    std::unique_lock<std::shared_mutex> lock(mutex_connections_);

    auto it = connections_.find(id);
    if(connections_.cend() != it)
        connections_.erase(it);
}

void ConnectionPool::erase(sole::uuid && id)
{
    std::unique_lock<std::shared_mutex> lock(mutex_connections_);

    auto it = connections_.find(std::forward<sole::uuid>(id));
    if(connections_.cend() != it)
        connections_.erase(it);
}

// Lookup
ConnectionPool::const_iterator ConnectionPool::find(sole::uuid const & id) const
{
    std::shared_lock<std::shared_mutex> lock(mutex_connections_);

    return connections_.find(id);
}

ConnectionPool::const_iterator ConnectionPool::find(sole::uuid && id) const
{
    std::shared_lock<std::shared_mutex> lock(mutex_connections_);

    return connections_.find(std::forward<sole::uuid>(id));
}

// Helper function to save connection to pools
void marklar_dataflow::connection::save(ConnectionPtr const & connection, OwnerPools const & pools)
{
    connection->emitter_pool_ = pools.emitter_pool_;
    if(pools.emitter_pool_)
        pools.emitter_pool_->insert(connection);

    connection->receiver_pool_ = pools.receiver_pool_;
    if(pools.receiver_pool_)
        pools.receiver_pool_->insert(connection);
}
