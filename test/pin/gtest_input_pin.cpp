#include "gtest/gtest.h"

/*
#include <pin/input_pin.hpp>

#include <condition_variable>
#include <string>

using marklar_dataflow::connection::Connection;
using marklar_dataflow::connection::ConnectionPtr;
using marklar_dataflow::connection::ConnectionPool;
using marklar_dataflow::connection::ConnectionPoolPtr;
using marklar_dataflow::error::Exception;
using marklar_dataflow::pin::Pin;
using marklar_dataflow::pin::InputPin;

namespace
{

TEST(gtest_pin_input_pin, input_pin_empty)
{
    constexpr size_t data_pushing = 7;

    std::condition_variable cv;
    InputPin<int> input_pin("test_input_pin", cv);

    EXPECT_TRUE(input_pin.empty());

    for(size_t i = 0; i < data_pushing; ++i)
    {
        input_pin.push(0);
    }

    EXPECT_FALSE(input_pin.empty());
}

TEST(gtest_pin_input_pin, input_pin_empty_pop)
{
    std::condition_variable cv;
    InputPin<int> input_pin("test_input_pin", cv);

    EXPECT_TRUE(input_pin.empty());
    EXPECT_THROW(input_pin.pop_front(), Exception);
    EXPECT_THROW(input_pin.pop_all(), Exception);
    EXPECT_THROW(input_pin.pop_back(), Exception);
}

TEST(gtest_pin_input_pin, input_pin_size)
{
    constexpr size_t data_pushing = 7;

    std::condition_variable cv;
    InputPin<int> input_pin("test_input_pin", cv);

    EXPECT_EQ(input_pin.size(), 0);

    for(size_t i = 0; i < data_pushing; ++i)
    {
        input_pin.push(0);
    }

    EXPECT_EQ(input_pin.size(), data_pushing);
}

TEST(gtest_pin_input_pin, input_pin_buffer_limits)
{
    constexpr size_t max_buffer_size = 7;
    constexpr size_t over_buffered = 27;

    std::condition_variable cv;
    InputPin<int> input_pin("test_input_pin", cv);

    input_pin.set_buffer_limit(max_buffer_size);

    EXPECT_EQ(input_pin.buffer_limit(), max_buffer_size);

    for(size_t i = 0; i < (max_buffer_size + over_buffered); ++i)
    {
        input_pin.push(0);
    }

    EXPECT_EQ(input_pin.size(), max_buffer_size);
}

TEST(gtest_pin_input_pin, input_pin_clear)
{
    constexpr size_t data_pushing = 7;

    std::condition_variable cv;
    InputPin<int> input_pin("test_input_pin", cv);

    EXPECT_EQ(input_pin.size(), 0);

    for(size_t i = 0; i < data_pushing; ++i)
    {
        input_pin.push(0);
    }

    EXPECT_EQ(input_pin.size(), data_pushing);

    input_pin.clear();

    EXPECT_EQ(input_pin.size(), 0);
}

TEST(gtest_pin_input_pin, input_pin_push_pop_front)
{
    constexpr int data_front = 82;
    constexpr int data_middle = 7;
    constexpr int data_back = 27;

    std::condition_variable cv;
    InputPin<int> input_pin("test_input_pin", cv);

    EXPECT_TRUE(input_pin.empty());

    input_pin.push(data_front);
    input_pin.push(data_middle);
    input_pin.push(data_back);
    const size_t num_of_data = input_pin.size();

    EXPECT_EQ(input_pin.pop_front(), data_front);
    EXPECT_EQ(input_pin.size(), num_of_data - 1);
}

TEST(gtest_pin_input_pin, input_pin_push_pop_all)
{
    constexpr int data_front = 82;
    constexpr int data_middle = 7;
    constexpr int data_back = 27;

    std::condition_variable cv;
    InputPin<int> input_pin("test_input_pin", cv);

    EXPECT_TRUE(input_pin.empty());

    input_pin.push(data_front);
    input_pin.push(data_middle);
    input_pin.push(data_back);
    const size_t num_of_data = input_pin.size();

    auto all_data = input_pin.pop_all();

    EXPECT_EQ(all_data.size(), num_of_data);
    EXPECT_TRUE(input_pin.empty());
}

TEST(gtest_pin_input_pin, input_pin_push_pop_back)
{
    constexpr int data_front = 82;
    constexpr int data_middle = 7;
    constexpr int data_back = 27;

    std::condition_variable cv;
    InputPin<int> input_pin("test_input_pin", cv);

    EXPECT_TRUE(input_pin.empty());

    input_pin.push(data_front);
    input_pin.push(data_middle);
    input_pin.push(data_back);
    const size_t num_of_data = input_pin.size();

    EXPECT_EQ(input_pin.pop_back(), data_back);
    EXPECT_EQ(input_pin.size(), num_of_data - 1);
}

TEST(gtest_pin_input_pin, input_pin_push_pop_simple)
{
    constexpr int data = 7;

    std::condition_variable cv;
    InputPin<int> input_pin("test_input_pin", cv);

    EXPECT_EQ(input_pin.size(), 0);

    input_pin.push(data);

    EXPECT_EQ(input_pin.pop_front(), data);
}

TEST(gtest_pin_input_pin, input_pin_push_pop_shared_ptr)
{
    using IntPtr = std::shared_ptr<int>;

    const IntPtr data = std::make_shared<int>(7);

    std::condition_variable cv;
    InputPin<IntPtr> input_pin("test_input_pin", cv);

    EXPECT_EQ(input_pin.size(), 0);

    input_pin.push(data);

    EXPECT_EQ(input_pin.pop_front(), data);
}

TEST(gtest_pin_input_pin, input_pin_is_ready_no_data)
{
    std::condition_variable cv;
    InputPin<int> input_pin("test_input_pin", cv);

    EXPECT_EQ(input_pin.flag(), (Pin::Flag::Accept | Pin::Flag::Publish)); // accept and publish data
    EXPECT_EQ((input_pin.flag() & Pin::Flag::Optional), Pin::Flag::None); // not optional pin

    EXPECT_FALSE(input_pin.is_ready());
}

TEST(gtest_pin_input_pin, input_pin_is_ready_has_data)
{
    constexpr int data = 7;

    std::condition_variable cv;
    InputPin<int> input_pin("test_input_pin", cv);

    EXPECT_EQ(input_pin.flag(), (Pin::Flag::Accept | Pin::Flag::Publish)); // accept and publish data
    EXPECT_EQ((input_pin.flag() & Pin::Flag::Optional), Pin::Flag::None); // not optional pin

    EXPECT_FALSE(input_pin.is_ready());
    input_pin.push(data);
    EXPECT_TRUE(input_pin.is_ready());
}

TEST(gtest_pin_input_pin, input_pin_is_ready_optional)
{
    constexpr int data = 7;

    std::condition_variable cv;
    InputPin<int> input_pin("test_input_pin", cv);

    input_pin.set_flag(Pin::Flag::Optional);
    EXPECT_EQ(input_pin.flag(), (Pin::Flag::Accept | Pin::Flag::Publish | Pin::Flag::Optional)); // optional pin what is accept and publish data

    EXPECT_TRUE(input_pin.is_ready());
    input_pin.push(data);
    EXPECT_TRUE(input_pin.is_ready());
}

TEST(gtest_pin_input_pin, input_pin_is_ready_not_publish)
{
    constexpr int data = 7;

    std::condition_variable cv;
    InputPin<int> input_pin("test_input_pin", cv);

    EXPECT_EQ(input_pin.flag(), (Pin::Flag::Accept | Pin::Flag::Publish)); // accept and publish data

    input_pin.push(data);
    EXPECT_TRUE(input_pin.is_ready());

    input_pin.unset_flag(Pin::Flag::Publish);
    EXPECT_FALSE(input_pin.is_ready());
}

TEST(gtest_pin_input_pin, input_pin_is_ready_not_publish_optional)
{
    constexpr int data = 7;

    std::condition_variable cv;
    InputPin<int> input_pin("test_input_pin", cv);

    input_pin.set_flag(Pin::Flag::Optional);
    EXPECT_EQ(input_pin.flag(), (Pin::Flag::Accept | Pin::Flag::Publish | Pin::Flag::Optional)); // optional pin what is accept and publish data

    input_pin.push(data);
    EXPECT_TRUE(input_pin.is_ready());

    input_pin.unset_flag(Pin::Flag::Publish);
    EXPECT_FALSE(input_pin.is_ready());
}

TEST(gtest_pin_input_pin, input_pin_flag_fixed)
{ // Allways publishing the last arrived data and only one data storaged
    constexpr int number_of_round = 7;

    std::condition_variable cv;
    InputPin<int> input_pin("test_input_pin", cv);

    input_pin.set_flag(Pin::Flag::Fixed);
    EXPECT_EQ(input_pin.flag(), (Pin::Flag::Accept | Pin::Flag::Publish | Pin::Flag::Fixed));

    for(int i = 0; i < number_of_round; ++i)
    {
        input_pin.push(i);
        EXPECT_EQ(input_pin.size(), 1);
    }

    for(int i = 0; i < number_of_round; ++i)
    {
        EXPECT_NO_THROW(input_pin.pop_front());
        EXPECT_EQ(input_pin.size(), 1);
    }
}

TEST(gtest_pin_input_pin, input_pin_flag_optional)
{ // Not required for node's valid operation
    constexpr int data = 7;

    std::condition_variable cv;
    InputPin<int> input_pin("test_input_pin", cv);

    input_pin.set_flag(Pin::Flag::Optional);
    EXPECT_EQ(input_pin.flag(), (Pin::Flag::Accept | Pin::Flag::Publish | Pin::Flag::Optional));

    EXPECT_TRUE(input_pin.is_ready());

    input_pin.push(data);
    EXPECT_TRUE(input_pin.is_ready());

    EXPECT_NO_THROW(input_pin.pop_front());
    EXPECT_TRUE(input_pin.is_ready());
}

TEST(gtest_pin_input_pin, input_pin_flag_single)
{ // Maximum 1 connection is allowed
    constexpr int number_of_connection = 7;

    std::condition_variable cv;
    InputPin<int> input_pin("test_input_pin", cv);

    EXPECT_EQ(input_pin.flag(), (Pin::Flag::Accept | Pin::Flag::Publish));
    EXPECT_EQ(input_pin.connection_pool()->size(), 0);

    for(int i = 0; i < number_of_connection; ++i)
    {
        wigwag::token aToken;
        ConnectionPtr connection = std::make_shared<Connection>(std::move(aToken));

        marklar_dataflow::connection::save(connection, {input_pin.connection_pool(), nullptr});
    }

    EXPECT_THROW(input_pin.set_flag(Pin::Flag::Single), Exception);
}

} // namespace
*/
