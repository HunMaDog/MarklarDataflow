#include "gtest/gtest.h"

#include <error/exception.hpp>

using marklar_dataflow::error::Exception;

namespace
{

TEST(gtest_error_exception, exception_throw_empty)
{
    EXPECT_THROW(throw Exception("", 0), Exception);
}

TEST(gtest_error_exception, exception_error_code)
{
    const int errorCode = 7;

    try
    {
        throw Exception("", errorCode);
    }
    catch(const Exception& e)
    {
        EXPECT_EQ(e.error_code(), errorCode);
    }
}

TEST(gtest_error_exception, exception_error_message)
{
    const std::string errorMessage = "Marklar";

    try
    {
        throw Exception(errorMessage, 0);
    }
    catch(const Exception& e)
    {
        EXPECT_EQ(e.what(), errorMessage);
    }
}

} // namespace
