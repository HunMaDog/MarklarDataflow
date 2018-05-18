#include "gtest/gtest.h"

#include <connection/connection.hpp>

using marklar_dataflow::connection::Connection;

namespace
{

TEST(gtest_connection_connection, connection_create_empty)
{
    wigwag::token aToken;
    Connection connection(std::move(aToken));

    EXPECT_NE(connection.id().str(), std::string());
}

TEST(gtest_connection_connection, connection_gen_uuid)
{
    wigwag::token aToken1;
    Connection connection1(std::move(aToken1));

    wigwag::token aToken2;
    Connection connection2(std::move(aToken2));

    EXPECT_NE(connection1.id().str(), connection2.id().str());
}

TEST(gtest_connection_connection, connection_token_save)

{
    constexpr int testInput = 7;
    int testOutput = 0;

    wigwag::signal<void(int)> signal;
    auto func = [&testOutput](int number){ testOutput = number; };

    std::unique_ptr<Connection> testConnection;

    {
        // Create WigWag connection in other scope to test token saved properly to Connection
        wigwag::token aToken = signal.connect(func);
        testConnection = std::make_unique<Connection>(std::move(aToken));
    }

    signal(testInput);
    EXPECT_EQ(testOutput, testInput);
}

TEST(gtest_connection_connection, connection_destruction)
{
    constexpr int testInputBefore = 7;
    constexpr int testInputAfter = 27;
    int testOutput = 0;

    wigwag::signal<void(int)> signal;
    auto func = [&testOutput](int number){ testOutput = number; };

    std::unique_ptr<Connection> testConnection;

    {
        wigwag::token aToken = signal.connect(func);
        testConnection = std::make_unique<Connection>(std::move(aToken));
    }

    // Without separated thread the signal emitted in the current thread
    signal(testInputBefore);
    EXPECT_EQ(testOutput, testInputBefore);

    // Release the connection, and after that signal is not allowed to change the output value
    testConnection.reset();
    // Without separated thread the signal emitted in the current thread
    signal(testInputAfter);
    EXPECT_NE(testOutput, testInputAfter);
}

} // namespace
