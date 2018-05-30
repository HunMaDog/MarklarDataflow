#pragma once

// Marklar headers
#include <attribute.hpp>
// 3rd party library headers
#include <sole.hpp>
// C++ standard Library headers
#include <string>

namespace marklar_dataflow
{
namespace policy
{
namespace identifier
{

class MARKLAR_EXPORT Uuid
{
public:
    Uuid(std::string const & name);
    virtual ~Uuid() = default;

    std::string & name();
    std::string const & name() const;

    sole::uuid const & id() const;

protected:
    sole::uuid const uuid_;

    std::string name_;
};

} // namespace identifier
} // namespace policy
} // namespace marklar_dataflow
