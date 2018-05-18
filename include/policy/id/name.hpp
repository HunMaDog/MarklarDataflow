#pragma once

// Marklar headers
#include <attribute.hpp>
// 3rd party library headers
// C++ standard Library headers
#include <string>

// Forward declarations
// Classes and structs
namespace marklar_dataflow
{
namespace policy
{
namespace id
{

class MARKLAR_EXPORT Name
{
public:
    Name(std::string const & name)
        : name_(name)
    {}
    virtual ~Name() noexcept = default;

    const std::string& name() const
    {
        return name_;
    }

protected:
    std::string name_;
};

} // namespace id
} // namespace policy
} // namespace marklar_dataflow
