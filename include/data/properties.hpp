#pragma once

// Marklar headers
#include <attribute.hpp>
#include <error/exception.hpp>
// 3rd party library headers
// C++ standard Library headers
#include <any>
#include <string>
#include <map>

// Forward declarations
namespace std
{
    class type_index;
}

// Classes and structs
namespace marklar_dataflow
{
namespace data
{

// TODO :: costume iterator

class MARKLAR_EXPORT Properties final
{
protected:
    typedef typename std::map<std::string, std::any> PropertiesMap;
    typedef typename PropertiesMap::const_iterator const_iterator;

public:
    // TODO :: rethink to change valid and check if has value or other function for check has value?
    bool contains(const std::string& key) const;

    std::type_index type(const std::string& key) const;

    template<typename _DataT>
    _DataT getAs(const std::string& key) const
    {
        auto it = data_.find(key);
        if (it == data_.cend())
            throw error::Exception("attribute not found", 0);

        try
        {
            return std::any_cast<_DataT>(it->second);
        }
        catch(const std::bad_any_cast& e)
        {
            throw error::Exception("conversion failed", 0);
        }
    }

    void set(const std::string& key, std::any && data);

    // Iterators
    const_iterator begin() const;
    const_iterator cbegin() const;

    const_iterator end() const;
    const_iterator cend() const;

private:
    PropertiesMap data_;
};

} // namespace data
} // namespace marklar_dataflow
