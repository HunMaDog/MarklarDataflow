#include "gtest/gtest.h"

#include <pin/pin.hpp>

#include <spdlog/spdlog.h>

#include <condition_variable>
#include <string>

using marklar_dataflow::pin::Pin;

namespace
{

class TestPin
    : public Pin
{
public:
    TestPin(std::string const & name, Flag const & flag = Flag::None)
        : Pin(name, flag)
    {}

    std::type_index data_type() const final
    {
        return typeid(this);
    }

    bool is_ready() const final
    {
        return is_ready_;
    }

    void set_flag(Flag const & flag) final
    {
        flag_ = flag_ | flag;
    }

    void unset_flag(Flag const & flag) final
    {
        flag_ = flag_ ^ flag;
    }


    void set_ready(bool ready)
    {
        is_ready_ = ready;
    }

private:
    bool is_ready_ = false;
};

TEST(gtest_pin_pin, pin_name_constructor)
{
    const std::string name = "Marklar";

    TestPin test_pin(name);
    EXPECT_EQ(test_pin.name(), name);
}

TEST(gtest_pin_pin, pin_name_change)
{
    const std::string nameBefore = "Marklar";
    const std::string nameAfter = "ralkraM";

    TestPin test_pin(nameBefore);
    EXPECT_EQ(test_pin.name(), nameBefore);

    test_pin.name() = nameAfter;
    EXPECT_EQ(test_pin.name(), nameAfter);
}

TEST(gtest_pin_pin, pin_flag_get)
{
    const std::string name = "Marklar";
    const Pin::Flag flag = Pin::Flag::Accept;

    TestPin test_pin(name, flag);
    EXPECT_EQ(test_pin.flag(), flag);
}

TEST(gtest_pin_pin, pin_logger_get)
{
    const std::string name = "Marklar";

    TestPin test_pin(name);
    EXPECT_EQ(test_pin.logger(), nullptr);
}

TEST(gtest_pin_pin, pin_logger_set)
{
    const std::string name = "Marklar";

    TestPin test_pin(name);
    EXPECT_EQ(test_pin.logger(), nullptr);

    test_pin.set_logger(spdlog::stdout_color_mt("console"));
    EXPECT_NE(test_pin.logger(), nullptr);
}

TEST(gtest_pin_pin, pin_flag_operator_or)
{
    Pin::Flag flag = Pin::Flag::None;
    EXPECT_EQ(flag, Pin::Flag::None);

    EXPECT_EQ((flag | Pin::Flag::Fixed), Pin::Flag::Fixed);
    EXPECT_NE((flag | Pin::Flag::Fixed), Pin::Flag::None);
}

TEST(gtest_pin_pin, pin_flag_operator_or_set)
{
    Pin::Flag flag = Pin::Flag::None;
    EXPECT_EQ(flag, Pin::Flag::None);

    flag |= Pin::Flag::Fixed;
    EXPECT_EQ(flag, Pin::Flag::Fixed);
    EXPECT_NE(flag, Pin::Flag::None);
}

TEST(gtest_pin_pin, pin_flag_operator_and)
{
    Pin::Flag flag = Pin::Flag::Fixed | Pin::Flag::Optional;
    EXPECT_EQ(flag, Pin::Flag::Fixed | Pin::Flag::Optional);

    EXPECT_EQ((flag & Pin::Flag::Fixed), Pin::Flag::Fixed);
    EXPECT_EQ((flag & Pin::Flag::Single), Pin::Flag::None);
}

TEST(gtest_pin_pin, pin_flag_operator_and_set)
{
    Pin::Flag flag = Pin::Flag::Fixed | Pin::Flag::Optional;
    EXPECT_EQ(flag, Pin::Flag::Fixed | Pin::Flag::Optional);

    flag &= Pin::Flag::Optional;
    EXPECT_EQ(flag, Pin::Flag::Optional);
    EXPECT_NE(flag, (Pin::Flag::Fixed | Pin::Flag::Optional));
}

TEST(gtest_pin_pin, pin_flag_operator_xor)
{
    Pin::Flag flag = Pin::Flag::Fixed;
    EXPECT_EQ(flag, Pin::Flag::Fixed);

    EXPECT_EQ((flag ^ Pin::Flag::Fixed), Pin::Flag::None);
    EXPECT_NE((flag ^ Pin::Flag::Fixed), Pin::Flag::Fixed);
}

TEST(gtest_pin_pin, pin_flag_operator_xor_set)
{
    Pin::Flag flag = Pin::Flag::Fixed;
    EXPECT_EQ(flag, Pin::Flag::Fixed);

    flag ^= Pin::Flag::Fixed;
    EXPECT_EQ(flag, Pin::Flag::None);
    EXPECT_NE(flag, Pin::Flag::Fixed);
}

} // namespace
