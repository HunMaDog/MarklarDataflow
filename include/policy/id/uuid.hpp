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
namespace id
{

class MARKLAR_EXPORT Uuid
{
public:
    Uuid(std::string const & name)
        : uuid_(sole::uuid4())
        , name_(name)
    {}
    virtual ~Uuid() = default;

    std::string & name()
    {
        return name_;
    }

    std::string const & name() const
    {
        return name_;
    }

    sole::uuid const & id() const
    {
        return uuid_;
    }

protected:
    sole::uuid uuid_;

    std::string name_;
};

} // namespace id
} // namespace policy
} // namespace marklar_dataflow
