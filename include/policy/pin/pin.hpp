#pragma once

// Marklar headers
#include <attribute.hpp>
#include <error/exception.hpp>
#include <pin/pin.hpp>
// 3rd party library headers
// C++ standard Library headers
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <utility>

// Forward declarations
// Classes and structs
namespace marklar_dataflow
{
namespace policy
{
namespace pin
{

// TODO :: costume iterator

class MARKLAR_EXPORT Pin
{
protected:
    typedef typename std::shared_ptr<marklar_dataflow::pin::Pin> PinPtr;
    typedef typename std::map<std::string, PinPtr> PinMap;
    typedef typename PinMap::const_iterator const_iterator;

public:
    Pin() {}
    virtual ~Pin() noexcept = default;

    // Element access
    template<typename _PinT>
    std::shared_ptr<_PinT> pin(std::string const & name)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_pins_);

        // TODO
        auto it = pins_.find(name);
        if(pins_.cend() == it)
            throw error::Exception("pin is not exists", 0);

        // TODO
        auto pin = std::dynamic_pointer_cast<_PinT>(it->second);
        if(!pin)
            throw error::Exception("pin data type is wrong", 0);

        return pin;
    }

    template<typename _PinT>
    std::shared_ptr<_PinT> pin(std::string && name)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_pins_);

        // TODO
        auto it = pins_.find(std::forward<std::string>(name));
        if(pins_.cend() == it)
            throw error::Exception("pin is not exists", 0);

        // TODO
        auto pin = std::dynamic_pointer_cast<_PinT>(it->second);
        if(!pin)
            throw error::Exception("pin data type is wrong", 0);

        return pin;
    }

    // Iterators
    const_iterator begin() const;
    const_iterator cbegin() const;

    const_iterator end() const;
    const_iterator cend() const;

    // Capacity
    bool empty() const;
    size_t size() const;

    // Modifiers
    void add_pin(PinPtr const & pin);
    void remove_pin(std::string const & name);

    // Lookup
    const_iterator find(std::string const & name) const;
    const_iterator find(std::string && name) const;

    // Checks
    bool is_ready() const;

private:
    mutable std::shared_mutex mutex_pins_;
    PinMap pins_;
};

} // namespace pin
} // namespace policy
} // namespace marklar_dataflow
