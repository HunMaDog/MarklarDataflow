#pragma once

// Marklar headers
#include <attribute.hpp>
#include <connection/connection_pool.hpp>
#include <error/exception.hpp>
#include <pin/pin.hpp>
// 3rd party library headers
// C++ standard Library headers
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>

// Classes and structs
namespace marklar_dataflow
{
namespace pin
{

template<typename _DataT>
class MARKLAR_EXPORT InputPin
    : public Pin
{
public:
    // TODO :: grouping params to reduce the number
    InputPin(
        std::string const & name
        , std::function<void()> && wake_up_func
        , Flag const & flag = (Flag::Accept | Flag::Publish)
    )
        : Pin(name, flag)
        , wake_up_func_(std::move(wake_up_func))
        , max_buffer_size_(10)
    {}
    ~InputPin() noexcept = default;

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

    _DataT pop_front()
    {
        // TODO
        if(Flag::None == (flag_ & Flag::Publish)) /* not publishing data */
            throw marklar_dataflow::error::Exception("pin is not publishing data", 0);

        std::unique_lock<std::shared_mutex> lock(mutex_buffer_);

        // TODO
        if (buffer_.empty())
            throw marklar_dataflow::error::Exception("no data available on pin", 0);

        _DataT data = buffer_[0];

        // only remove data if the pin is not fixed
        if(Flag::None == (flag_ & Flag::Fixed))
            buffer_.erase(buffer_.begin());

        // Prevent big memory usage
        if(buffer_.capacity() > max_buffer_size_)
            buffer_.shrink_to_fit();

        return data;
    }

    std::vector<_DataT> pop_all()
    {
        // TODO
        if(Flag::None == (flag_ & Flag::Publish)) /* not publishing data */
            throw marklar_dataflow::error::Exception("pin is not publishing data", 0);

        std::unique_lock<std::shared_mutex> lock(mutex_buffer_);

        // TODO
        if (buffer_.empty())
            throw marklar_dataflow::error::Exception("no data available on pin", 0);

        std::vector<_DataT> all_data(buffer_.begin(), buffer_.end());

        // only remove data if the pin is not fixed
        if(Flag::None == (flag_ & Flag::Fixed))
            buffer_.clear();

        // Prevent big memory usage
        if(buffer_.capacity() > max_buffer_size_)
            buffer_.shrink_to_fit();

        return all_data;
    }

    _DataT pop_back()
    {
        // TODO
        if(Flag::None == (flag_ & Flag::Publish)) /* not publishing data */
            throw marklar_dataflow::error::Exception("pin is not publishing data", 0);

        std::unique_lock<std::shared_mutex> lock(mutex_buffer_);

        // TODO
        if (buffer_.empty())
            throw marklar_dataflow::error::Exception("no data available on pin", 0);

        _DataT data = buffer_[buffer_.size() - 1];

        // only remove data if the pin is not fixed
        if(Flag::None == (flag_ & Flag::Fixed))
            buffer_.erase((--buffer_.end()));

        // Prevent big memory usage
        if(buffer_.capacity() > max_buffer_size_)
            buffer_.shrink_to_fit();

        return data;
    }

    void push(_DataT const & data)
    {
        if (Flag::None == (flag_ & Flag::Accept)) /* not accepting data */
            return;

        std::unique_lock<std::shared_mutex> lock(mutex_buffer_);

        buffer_.emplace_back(std::move(data));

        // if pin is fixed than only allowed 1 element
        if(Flag::Fixed == (flag_ & Flag::Fixed))
            buffer_.erase(buffer_.cbegin(), buffer_.cend() - 1);
        else if(buffer_.size() > max_buffer_size_)
        {
            buffer_.erase(buffer_.cbegin()+max_buffer_size_, buffer_.cend());
            // TODO log
//            logger_->log(spdlog::level::info, "data droped");
        }

        if(Flag::None == (flag_ & Flag::Silent) && wake_up_func_)
            wake_up_func_();
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
                /* don't know which data is need to be fixed till new data arrived */
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
        if(Flag::None == (flag_ & Flag::Publish)) /* not publishing data */
            return false;

        std::shared_lock lock(mutex_buffer_);

        if (
            buffer_.empty() /* not empty */
            && Flag::None == (flag_ & Flag::Optional) /* not optional */
        )
            return false;
        else
            return true;
    }

private:
    // TODO :: elnevezes, ellenorzesek, constructorba, outputra is atvinni, etc.
    std::function<void()> wake_up_func_;

    mutable std::shared_mutex mutex_buffer_;
    std::atomic<size_t> max_buffer_size_;
    std::vector<_DataT> buffer_;
};

template<typename _DataT>
std::shared_ptr<InputPin<_DataT>> InputPinFactory(
    std::string const & name
    , std::condition_variable & condition
    , Pin::Flag const & flag = (Pin::Flag::Accept | Pin::Flag::Publish)
)
{
    return std::make_shared<InputPin<_DataT>>(name, condition, flag);
}

} // namespace pin
} // namespace marklar_dataflow
