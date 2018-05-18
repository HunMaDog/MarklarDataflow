#include "gtest/gtest.h"

/*
#include <pin/output_pin.hpp>

using marklar_dataflow::connection::Connection;
using marklar_dataflow::connection::ConnectionPtr;
using marklar_dataflow::connection::ConnectionPool;
using marklar_dataflow::connection::ConnectionPoolPtr;
using marklar_dataflow::error::Exception;
using marklar_dataflow::pin::Pin;
using marklar_dataflow::pin::InputPin;
using marklar_dataflow::pin::OutputPin;

namespace
{

TEST(gtest_pin_output_pin, output_pin_connect_func)
{
    constexpr int testInput = 7;
    int testOutput = 0;

    std::condition_variable cv;
    OutputPin<int> output_pin("test_output_pin", cv);

    EXPECT_NO_THROW(output_pin += [&testOutput](int number){ testOutput = number; });

    EXPECT_NO_THROW(output_pin.push(testInput));

    EXPECT_EQ(testOutput, testInput);
}

*TEST(gtest_pin_output_pin, output_pin_connect_input_pin)
{
    constexpr int testInput = 7;
    int testOutput = 0;

    std::condition_variable cv;
    auto output_pin = std::make_shared<OutputPin<int>>("test_output_pin", cv);
    auto input_pin = std::make_shared<InputPin<int>>("test_input_pin", cv);

//    output_pin += input_pin;
    EXPECT_NO_THROW(output_pin->connect(input_pin));

    EXPECT_NO_THROW(output_pin->push(testInput));
    EXPECT_NO_THROW(testOutput = input_pin->pop());

    EXPECT_EQ(testOutput, testInput);
}*

} // namespace
*/
