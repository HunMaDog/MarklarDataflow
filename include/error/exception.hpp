#pragma once

// Marklar headers
#include <attribute.hpp>
// 3rd party library headers
// C++ standard Library headers
#include <system_error>
#include <stdexcept>

// Forward declarations
// Classes and structs
namespace marklar_dataflow
{
namespace error
{

class MARKLAR_EXPORT Exception
   : public std::runtime_error
{
public:
    Exception(std::string const & what, int error_code)
        :  runtime_error(what)
        ,  error_code_(error_code)
    {}

    int error_code() const
    {
        return error_code_;
    }

private:
    int error_code_ = 0;
};

} // namespace error
} // marklar_dataflow
