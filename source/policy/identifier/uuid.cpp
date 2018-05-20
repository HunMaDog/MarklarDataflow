#include <policy/identifier/uuid.hpp>

// Marklar headers
// 3rd party library headers
// C++ standard Library headers

using marklar_dataflow::policy::identifier::Uuid;

Uuid::Uuid(std::string const & name)
    : uuid_(sole::uuid4())
    , name_(name)
{}

std::string & Uuid::name()
{
    return name_;
}

std::string const & Uuid::name() const
{
    return name_;
}

sole::uuid const & Uuid::id() const
{
    return uuid_;
}
