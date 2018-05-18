#include "gtest/gtest.h"

#include <utils/string/makestring.hpp>

using marklar_dataflow::utils::string::MakeString;

namespace
{

TEST(gtest_utils_string_makestring, makestring_empty)
{
    const std::string makeStr = MakeString();

    EXPECT_EQ(makeStr, std::string());
}

/*
TEST(gtest_utils_string_makestring, makestring_char_ptr)
{
    const std::vector<char> chars = {'M', 'a', 'r', 'k', 'l', 'a', 'r'};
    const std::string charsStr = std::string(chars.data());

    const std::string makeStr = MakeString() << chars.data();

    EXPECT_EQ(makeStr, charsStr);
}
*/

TEST(gtest_utils_string_makestring, makestring_string)
{
    const std::string str = std::string("Marklar");

    const std::string makeStr = MakeString() << str;

    EXPECT_EQ(makeStr, str);
}

TEST(gtest_utils_string_makestring, makestring_int)
{
    const int number = 7;
    const std::string numberStr = std::to_string(number);

    const std::string makeStr = MakeString() << number;

    EXPECT_EQ(makeStr, numberStr);
}

TEST(gtest_utils_string_makestring, makestring_double)
{
    const double number = 3.14159265359;
    const std::string numberStr = std::to_string(number);

    const std::string makeStr = MakeString().precision(7) << number;

    EXPECT_EQ(makeStr, numberStr);
}

TEST(gtest_utils_string_makestring, makestring_flag_hex)
{
    const int number = 16;
    std::stringstream numberSStr;
    numberSStr << std::hex << number;

    const std::string makeStr = MakeString().flag(std::ios_base::hex) << number;

    EXPECT_EQ(makeStr, numberSStr.str());
}

} // namespace
