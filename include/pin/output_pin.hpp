#pragma once

// Marklar headers
#include <attribute.hpp>
#include <connection/connection_pool.hpp>
#include <error/exception.hpp>
#include <pin/input_pin.hpp>
#include <pin/pin.hpp>
// 3rd party library headers
#include <wigwag/signal.hpp>
// C++ standard Library headers
#include <condition_variable>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <utility>
#include <vector>

// Classes and structs
namespace marklar_dataflow
{
namespace pin
{

template<typename _DataT>
class MARKLAR_EXPORT OutputPin final
    : public Pin
{
protected:
    // TODO :: connection friend(s)

public:
    // TODO :: grouping params to reduce the number
    OutputPin(
        std::string const & name
        , std::function<void()> && wake_up_func
        , Flag const & flag = (Flag::Accept | Flag::Publish)
    )
        : Pin(name, flag)
        , wake_up_func_(std::move(wake_up_func))
        , max_buffer_size_(10)
    {}
    ~OutputPin() noexcept = default;

    // Element access
    size_t buffer_limit() const
    {
        return max_buffer_size_;
    }

    // Capacity
    bool empty() const
    {
        std::shared_lock lock(mutex_buffer_);

        return buffer_.empty();

    }

    size_t size() const
    {
        std::shared_lock lock(mutex_buffer_);

        return buffer_.size();
    }

    // Modifiers
    void set_buffer_limit(size_t const & maxBufferSize)
    {
        max_buffer_size_ = maxBufferSize;
    }

    void clear()
    {
        std::unique_lock<std::shared_mutex> lock(mutex_buffer_);

        buffer_.clear();
    }

    void push(_DataT const & data)
    {
        if(Flag::None == (flag_ & Flag::Accept)) /* not accepting data */
            return;

        std::unique_lock<std::shared_mutex> lock(mutex_buffer_);

        buffer_.emplace_back(std::move(data));

        // if pin is fixed than only allowed 1 element
        if(Flag::Fixed == (flag_ & Flag::Fixed))
            buffer_.erase(buffer_.cbegin(), buffer_.cend() - 1);
        else if(buffer_.size() > max_buffer_size_)
            buffer_.erase(buffer_.cbegin()+max_buffer_size_, buffer_.cend());

        if(Flag::None == (flag_ & Flag::Publish)) /* not sending data */
            return;

//        for (auto& d : buffer_)
        for (auto d : buffer_)
            push_signal_(d);

        if(Flag::None == (flag_ & Flag::Fixed))
            buffer_.clear();
    }

    // Connectors
    sole::uuid const & connect(
        std::function<void(_DataT const &)> const & func
        , connection::TokenProperties const & properties = {}
    )
    {
        return connect_func(func, properties);
    }

    sole::uuid const & connect(
        std::shared_ptr<InputPin<_DataT>> const & input_pin
        , connection::TokenProperties const & properties = {}
    )
    {
        return connect_pin(input_pin, properties);
    }

    void operator+=(std::function<void(_DataT const &)> const & func)
    {
        connect_func(func);
    }

    void operator+=(std::shared_ptr<InputPin<_DataT>> const & input_pin)
    {
        connect_pin(input_pin);
    }

    // Remove connections
    void disconnect(std::shared_ptr<InputPin<_DataT>> const & input_pin)
    {
        disconnect_pin(input_pin);
    }

    void operator-=(std::shared_ptr<InputPin<_DataT>> const & input_pin)
    {
        disconnect_pin(input_pin);
    }

    // Overrides
    std::type_index data_type() const final
    {
        return typeid(_DataT);
    }

    void set_flag(Flag const & flag) final
    {
        switch(flag)
        {
            case Flag::Fixed :
            {
                // TODO
                /* don't know which data is need to be fixed till new data sending */
                if(size() > 1)
                    throw error::Exception("pin more then one data contains", 0);

                break;
            }

            case Flag::Single :
            {
                // TODO
                /* already violate the new flag */
                if(connection_pool_->size() > 1)
                    throw error::Exception("pin has more than one connection", 0);

                break;
            }

            case Flag::Accept :
            case Flag::Publish :
            case Flag::Optional :
            case Flag::Locked :
            {
                break;
            }

            default :
            {
                // TODO
                throw error::Exception("unsupported flag", 0);
            }
        }

        flag_ = flag_ | flag;
    }

    void unset_flag(Flag const & flag) final
    {
        flag_ = flag_ ^ flag;
    }

    bool is_ready() const final
    {
        if(Flag::None == (flag_ & Flag::Accept)) /* not accepting data */
            return false;

        if(
            connection_pool_->empty()
            && Flag::None == (flag_ & Flag::Optional)
        )
            return false;

        return true;
    }

protected:
    sole::uuid const & connect_func(
        std::function<void(_DataT const &)> const & func
        , connection::TokenProperties const & properties = {}
    )
    {
        if(!connectable())
            throw error::Exception("not allowed connectiong to emitter pin", 0);

        wigwag::token token;
        if(properties.worker_)
            token = push_signal_.connect(properties.worker_, func, properties.handler_);
        else
            token = push_signal_.connect(func, properties.handler_);

        auto new_connection = std::make_shared<connection::Connection>(std::forward<wigwag::token>(token));
        connection::save(new_connection, { connection_pool_, nullptr } );

        // TODO :: redesign/rethink
        if(
            Flag::Fixed == (flag_ & Flag::Fixed)
            && Flag::Publish == (flag_ & Flag::Publish)
        )
        {
            std::unique_lock<std::shared_mutex> lock(mutex_buffer_);

//            for (auto& d : buffer_)
            for (auto d : buffer_)
                func(d);
        }

        if(Flag::None == (flag_ & Flag::Silent) && wake_up_func_)
            wake_up_func_();

        return new_connection->id();
    }

    // TODO :: template parameters for adding wigwag specific connection options
    sole::uuid const & connect_pin(
        std::shared_ptr<InputPin<_DataT>> const & input_pin
        , connection::TokenProperties const & properties = {}
    )
    {
        if(!connectable())
            throw error::Exception("not allowed connectiong to emitter pin", 0);

        if(!input_pin->connectable())
            throw error::Exception("not allowed connectiong to receiver pin", 0);

        wigwag::token token;
        if(properties.worker_)
            token = push_signal_.connect([input_pin](_DataT const & data){ input_pin->push(data); }, properties.handler_);
        else
            token = push_signal_.connect(properties.worker_, [input_pin](_DataT const & data){ input_pin->push(data); }, properties.handler_);

        auto new_connection = std::make_shared<connection::Connection>(std::forward<wigwag::token>(token));
        connection::save(new_connection, { connection_pool_, input_pin->connection_pool() } );

        // TODO :: redesign/rethink
        if(
            Flag::Fixed == (flag_ & Flag::Fixed)
            && Flag::Publish == (flag_ & Flag::Publish)
        )
        {
            std::unique_lock<std::shared_mutex> lock(mutex_buffer_);

            for (auto& d : buffer_)
                input_pin->push(d);
        }

        if(Flag::None == (flag_ & Flag::Silent) && wake_up_func_)
            wake_up_func_();

        return new_connection->id();
    }

    void disconnect_pin(std::shared_ptr<InputPin<_DataT>> const & input_pin)
    {
        for(auto it = connection_pool_->cbegin(); it != connection_pool_->cend(); ++it)
        {
            auto connection = it->second;

            if(connection->receiver_pool().lock() == input_pin->connection_pool())
                connection->disconnect();
        }
    }

private:
    std::function<void()> wake_up_func_;

    mutable std::shared_mutex mutex_buffer_;
    std::atomic<size_t> max_buffer_size_;
    std::vector<_DataT> buffer_;

    wigwag::signal<void(_DataT const &)> push_signal_;
};

template<typename _DataT>
std::shared_ptr<OutputPin<_DataT>> OutputPinFactory(
    std::string const & name
    , std::condition_variable & condition
    , Pin::Flag const & flag = (Pin::Flag::Accept | Pin::Flag::Publish)
)
{
    return std::make_shared<OutputPin<_DataT>>(name, condition, flag);
}

} // namespace pin
} // namespace marklar_dataflow
