#include <command/dashboard.hpp>

// Marklar headers
// 3rd party library headers
// C++ standard Library headers

using marklar_dataflow::command::Dashboard;

void Dashboard::passthrough()
{
    std::unique_lock<std::mutex> lock(mutex_dashboard_);

    passthrough_impl();
}

void Dashboard::pause()
{
    std::unique_lock<std::mutex> lock(mutex_dashboard_);

    pause_impl();
}

void Dashboard::start()
{
    std::unique_lock<std::mutex> lock(mutex_dashboard_);

    start_impl();
}

void Dashboard::stop()
{
    std::unique_lock<std::mutex> lock(mutex_dashboard_);

    stop_impl();
}
