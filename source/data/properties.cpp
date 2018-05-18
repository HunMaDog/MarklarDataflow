#include <data/properties.hpp>

#include <typeindex>
#include <typeinfo>

using namespace marklar_dataflow::data;

bool Properties::contains(const std::string& key) const
{
    return (data_.find(key) != data_.cend());
}

std::type_index Properties::type(const std::string& key) const
{
    auto it = data_.find(key);
    if (it == data_.cend())
        throw error::Exception("attribute is not found", 0);

    return std::type_index(it->second.type());
}

void Properties::set(const std::string& key, std::any && data)
{
    if (contains(key))
        throw error::Exception("attribute is already exists", 0);

    data_[key] = std::move(data);
}

Properties::const_iterator Properties::begin() const
{
    return data_.begin();
}

Properties::const_iterator Properties::cbegin() const
{
    return data_.cbegin();
}

Properties::const_iterator Properties::end() const
{
    return data_.end();
}

Properties::const_iterator Properties::cend() const
{
    return data_.cend();
}
