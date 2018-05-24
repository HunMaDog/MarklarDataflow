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
namespace identifier
{

class MARKLAR_EXPORT Name
{
public:
    Name(std::string const & name);
    virtual ~Name() noexcept = default;

    const std::string& name() const;

protected:
    std::string name_;
};

} // namespace identifier
} // namespace policy
} // namespace marklar_dataflow
