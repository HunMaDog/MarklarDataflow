#pragma once

// Marklar headers
#include <attribute.hpp>
#include <connection/connection.hpp>
// 3rd party library headers
#include <sole.fwd.hpp>
// C++ standard Library headers
#include <map>
#include <memory>
#include <shared_mutex>
#include <vector>

// Forward declarations
namespace marklar_dataflow
{
namespace connection
{

void MARKLAR_EXPORT save(ConnectionPtr const & connection, OwnerPools const & pools);

} // namespace connection
} // namespace marklar_dataflow

// Classes and structs
namespace marklar_dataflow
{
namespace connection
{

class MARKLAR_EXPORT ConnectionPool
{
protected:
    typedef typename std::map<sole::uuid, ConnectionPtr> ConnectionMap;
    typedef typename ConnectionMap::const_iterator const_iterator;

    friend void save(ConnectionPtr const & connection, OwnerPools const & pools);
    friend void Connection::disconnect() const;

public:
    ConnectionPool() =  default;
    ConnectionPool(ConnectionPool const & ) noexcept =  delete;

    ~ConnectionPool();

    // Element access
    ConnectionPtr const connection(sole::uuid const & id) const; // with bounds checking
    ConnectionPtr const connection(sole::uuid && id) const; // with bounds checking

    // Iterators
    const_iterator begin() const;
    const_iterator cbegin() const;

    const_iterator end() const;
    const_iterator cend() const;

    // Capacity
    bool empty() const;
    size_t size() const;

    // Modifiers
    void clear();

protected:
    void insert(ConnectionPtr const & connection);
    void insert(ConnectionPtr && id);

    void erase(sole::uuid const & id);
    void erase(sole::uuid && id);

public:
    // Lookup
    const_iterator find(sole::uuid const & id) const;
    const_iterator find(sole::uuid && id) const;

private:
    mutable std::shared_mutex mutex_connections_;

    ConnectionMap connections_;
};

} // namespace connection
} // namespace marklar_dataflow
