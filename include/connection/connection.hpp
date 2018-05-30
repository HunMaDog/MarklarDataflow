#pragma once

// Marklar headers
#include <attribute.hpp>
// 3rd party library headers
//#include <uuid.h>
#include <sole.hpp>
#include <wigwag/signal.hpp>
// C++ standard Library headers
#include <memory>
#include <mutex>

// Forward declarations
namespace marklar_dataflow
{
namespace connection
{

class ConnectionPool;
typedef std::shared_ptr<ConnectionPool> ConnectionPoolPtr;

class Connection;
typedef std::shared_ptr<Connection> ConnectionPtr;

struct OwnerPools;

void MARKLAR_EXPORT save(ConnectionPtr const & connection, OwnerPools const & pools);

} // namespace connection
} // namespace marklar_dataflow

// Classes and structs
namespace marklar_dataflow
{
namespace connection
{

struct MARKLAR_EXPORT OwnerPools
{
    ConnectionPoolPtr const & emitter_pool_;
    ConnectionPoolPtr const & receiver_pool_;
};

struct MARKLAR_EXPORT TokenProperties
{
    std::shared_ptr<wigwag::task_executor> worker_ = nullptr;
    wigwag::handler_attributes handler_ = wigwag::handler_attributes::none;
};

class MARKLAR_EXPORT Connection
{
protected:
    friend void save(ConnectionPtr const & connection, OwnerPools const & pools);

public:
    Connection(wigwag::token && token);
    Connection(Connection const &) noexcept = delete;

    // Identifiers
    sole::uuid const & id() const;

    // Element access
    std::weak_ptr<ConnectionPool> const emitter_pool() const;
    std::weak_ptr<ConnectionPool> const receiver_pool() const;

    // Modifiers
    void disconnect() const;

protected:
    mutable std::mutex mutex_disconnect_;

    // TODO :: const and set by constructor
    std::weak_ptr<ConnectionPool> emitter_pool_;
    std::weak_ptr<ConnectionPool> receiver_pool_;

private:
    sole::uuid const uuid_;
    wigwag::token const token_;
};

} // namespace connection
} // namespace marklar_dataflow
