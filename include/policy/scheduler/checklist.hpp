#pragma once

// Marklar headers
#include <attribute.hpp>
// 3rd party library headers
// C++ standard Library headers
#include <atomic>
#include <functional>
#include <vector>

namespace marklar_dataflow
{
namespace policy
{
namespace scheduler
{

class MARKLAR_EXPORT Checklist
{
public:
    Checklist(std::atomic<bool> & running);
    virtual ~Checklist() noexcept = default;

protected:
    void push_check(std::function<bool(void)> && checkFunction);

    void preparation();
    void termination();

    bool is_ready();

private:
    std::atomic<bool> & running_;

    std::vector<std::function<bool(void)>> checks_;
};

} // namespace scheduler
} // namespace policy
} // namespace marklar_dataflow
