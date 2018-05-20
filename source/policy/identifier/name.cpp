#include <policy/identifier/name.hpp>

// Marklar headers
// 3rd party library headers
// C++ standard Library headers

using marklar_dataflow::policy::identifier::Name;

Name::Name(std::string const & name)
    : name_(name)
{}

const std::string& Name::name() const
{
    return name_;
}
