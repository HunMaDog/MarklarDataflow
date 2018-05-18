#pragma once

// Marklar headers
#include <attribute.hpp>
// 3rd party library headers
// C++ standard Library headers
#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <string>
#include <thread>

// Forward declarations
// Classes and structs
namespace marklar_dataflow
{
namespace node
{

class MARKLAR_EXPORT Controller
{
public:
    Controller(std::atomic<bool> & running);

    void notify();

protected:
    Controller & execute();

    std::future<std::atomic<bool> &> futureWait_;

private:
    std::atomic<bool> & running_;
};

} // namespace command
} // namespace marklar_dataflow
