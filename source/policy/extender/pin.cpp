#include <policy/extender/pin.hpp>

// Marklar headers
// 3rd party library headers
// C++ standard Library headers

using marklar_dataflow::policy::extender::Pin;
using marklar_dataflow::error::Exception;

Pin::const_iterator Pin::begin() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_pins_);

    return pins_.begin();
}

Pin::const_iterator Pin::cbegin() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_pins_);

    return pins_.cbegin();
}

Pin::const_iterator Pin::end() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_pins_);

    return pins_.end();
}

Pin::const_iterator Pin::cend() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_pins_);

    return pins_.cend();
}

bool Pin::empty() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_pins_);

    return pins_.empty();
}

size_t Pin::size() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_pins_);

    return pins_.size();
}

void Pin::add_pin(Pin::PinPtr const & pin)
{
    std::unique_lock<std::shared_mutex> lock(mutex_pins_);

    auto const & name = pin->name();

    // TODO
    auto it = pins_.find(name);
    if(pins_.cend() != it)
        throw error::Exception("pin name is already exists", 0);

    pins_[name] = pin;
}

void Pin::remove_pin(std::string const & name)
{
    std::unique_lock<std::shared_mutex> lock(mutex_pins_);

    auto it = pins_.find(name);
    if(it != pins_.cend())
        pins_.erase(it);
}

Pin::const_iterator Pin::find(std::string const & name) const
{
    std::shared_lock<std::shared_mutex> lock(mutex_pins_);

    return pins_.find(name);
}

Pin::const_iterator Pin::find(std::string && name) const
{
    std::shared_lock<std::shared_mutex> lock(mutex_pins_);

    return pins_.find(std::forward<std::string>(name));
}

bool Pin::is_ready() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_pins_);

    for (auto it = pins_.cbegin(); it != pins_.cend(); ++it)
    {
        if (!it->second->is_ready())
            return false;
    }

    return true;
}
