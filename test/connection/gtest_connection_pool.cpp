#include "gtest/gtest.h"

#include <connection/connection_pool.hpp>

using marklar_dataflow::connection::Connection;
using marklar_dataflow::connection::ConnectionPtr;
using marklar_dataflow::connection::ConnectionPool;
using marklar_dataflow::connection::ConnectionPoolPtr;

namespace
{

TEST(gtest_connection_connection_pool, connection_pool_empty)
{
    ConnectionPoolPtr connection_pool = std::make_shared<ConnectionPool>();

    EXPECT_TRUE(connection_pool->empty());
}

TEST(gtest_connection_connection_pool, connection_pool_insert)
{
    constexpr int numberOfConnection = 7;

    ConnectionPoolPtr emitter_pool = std::make_shared<ConnectionPool>();
    ConnectionPoolPtr receier_pool = std::make_shared<ConnectionPool>();

    for(int i = 0; i < numberOfConnection; ++i)
    {
        wigwag::token aToken;
        ConnectionPtr connection = std::make_shared<Connection>(std::move(aToken));

        marklar_dataflow::connection::save(connection, {emitter_pool, receier_pool});
    }

    EXPECT_EQ(emitter_pool->size(), numberOfConnection);
    EXPECT_EQ(receier_pool->size(), numberOfConnection);
}

TEST(gtest_connection_connection_pool, connection_pool_connection)
{
    constexpr int numberOfConnection = 7;

    ConnectionPoolPtr emitter_pool = std::make_shared<ConnectionPool>();
    ConnectionPoolPtr receier_pool = std::make_shared<ConnectionPool>();

    std::vector<sole::uuid> ids;

    for(int i = 0; i < numberOfConnection; ++i)
    {
        wigwag::token aToken;
        ConnectionPtr connection = std::make_shared<Connection>(std::move(aToken));
        ids.emplace_back(connection->id());

        marklar_dataflow::connection::save(connection, {emitter_pool, receier_pool});
    }

    EXPECT_EQ(emitter_pool->size(), numberOfConnection);
    EXPECT_EQ(receier_pool->size(), numberOfConnection);

    for(int i = 0; i < numberOfConnection; ++i)
    {
        EXPECT_EQ(emitter_pool->connection(ids[i])->id().str(), ids[i].str());
        EXPECT_EQ(receier_pool->connection(ids[i])->id().str(), ids[i].str());
    }
}

TEST(gtest_connection_connection_pool, connection_pool_erase)
{
    constexpr int numberOfConnection = 7;
    constexpr int numberOfErase = 5;

    EXPECT_GE(numberOfConnection, numberOfErase);

    ConnectionPoolPtr emitter_pool = std::make_shared<ConnectionPool>();
    ConnectionPoolPtr receier_pool = std::make_shared<ConnectionPool>();

    std::vector<sole::uuid> ids;

    for(int i = 0; i < numberOfConnection; ++i)
    {
        wigwag::token aToken;
        ConnectionPtr connection = std::make_shared<Connection>(std::move(aToken));
        ids.emplace_back(connection->id());

        marklar_dataflow::connection::save(connection, {emitter_pool, receier_pool});
    }

    EXPECT_EQ(emitter_pool->size(), numberOfConnection);
    EXPECT_EQ(receier_pool->size(), numberOfConnection);

    for(int i = 0; i < numberOfErase; ++i)
    {
        emitter_pool->connection(ids[i])->disconnect();
    }

    EXPECT_EQ(emitter_pool->size(), numberOfConnection - numberOfErase);
    EXPECT_EQ(receier_pool->size(), numberOfConnection - numberOfErase);
}

TEST(gtest_connection_connection_pool, connection_pool_clear)
{
    constexpr int numberOfConnection = 7;

    ConnectionPoolPtr emitter_pool = std::make_shared<ConnectionPool>();
    ConnectionPoolPtr receier_pool = std::make_shared<ConnectionPool>();

    std::vector<sole::uuid> ids;

    for(int i = 0; i < numberOfConnection; ++i)
    {
        wigwag::token aToken;
        ConnectionPtr connection = std::make_shared<Connection>(std::move(aToken));
        ids.emplace_back(connection->id());

        marklar_dataflow::connection::save(connection, {emitter_pool, receier_pool});
    }

    EXPECT_EQ(emitter_pool->size(), numberOfConnection);
    EXPECT_EQ(receier_pool->size(), numberOfConnection);

    emitter_pool->clear();

    EXPECT_EQ(emitter_pool->size(), 0);
    EXPECT_EQ(receier_pool->size(), 0);
}

TEST(gtest_connection_connection_pool, connection_pool_iterator_begin_end)
{
    constexpr int numberOfConnection = 7;

    ConnectionPoolPtr emitter_pool = std::make_shared<ConnectionPool>();

    for(int i = 0; i < numberOfConnection; ++i)
    {
        wigwag::token aToken;
        ConnectionPtr connection = std::make_shared<Connection>(std::move(aToken));

        marklar_dataflow::connection::save(connection, {emitter_pool, nullptr});
    }

    EXPECT_EQ(emitter_pool->size(), numberOfConnection);

    size_t counter = 0;
    auto it = emitter_pool->begin();
    while(emitter_pool->end() != it)
    {
        ++counter;
        ++it;
    }

    EXPECT_EQ(counter, numberOfConnection);
}

TEST(gtest_connection_connection_pool, connection_pool_iterator_cbegin_cend)
{
    constexpr int numberOfConnection = 7;

    ConnectionPoolPtr emitter_pool = std::make_shared<ConnectionPool>();

    for(int i = 0; i < numberOfConnection; ++i)
    {
        wigwag::token aToken;
        ConnectionPtr connection = std::make_shared<Connection>(std::move(aToken));

        marklar_dataflow::connection::save(connection, {emitter_pool, nullptr});
    }

    EXPECT_EQ(emitter_pool->size(), numberOfConnection);

    size_t counter = 0;
    for(auto it = emitter_pool->cbegin(); emitter_pool->cend() != it; ++it)
    {
        ++counter;
    }

    EXPECT_EQ(counter, numberOfConnection);
}

} // namespace
