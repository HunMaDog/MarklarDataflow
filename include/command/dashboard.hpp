#pragma once

// Marklar headers
#include <attribute.hpp>
// 3rd party library headers
// C++ standard Library headers
#include <mutex>

// Forward declarations
// Classes and structs
namespace marklar_dataflow
{
namespace command
{

class MARKLAR_EXPORT Dashboard
{
public:
    virtual ~Dashboard() noexcept = default;

    void passthrough();

    void pause();

    void start();

    void stop();

protected:
    virtual void passthrough_impl() = 0;

    virtual void pause_impl() = 0;

    virtual void start_impl() = 0;

    virtual void stop_impl() = 0;

private:
    std::mutex mutex_dashboard_;
};

} // namespace command
} // namespace marklar_dataflow
