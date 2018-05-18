#pragma once

// Marklar headers
#include <attribute.hpp>
#include <error/exception.hpp>
// 3rd party library headers
// C++ standard Library headers
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <vector>

namespace marklar_dataflow
{
namespace policy
{
namespace wait
{

class MARKLAR_EXPORT Checklist
{
public:
    Checklist(std::atomic<bool> & running)
        : running_(running)
    {}
    virtual ~Checklist() noexcept = default;

    void push_check(std::function<bool(void)> && checkFunction)
    {
        checks_.emplace_back(std::move(checkFunction));
    }

protected:
    void preparation()
    {
        if(checks_.empty())
            throw error::Exception("empty wait checks", 0);
    }

    void termination()
    {}

    bool is_ready()
    {
        for(const auto& checking : checks_)
            if(!checking() && running_)
                return false;

        return running_;
    }

private:
    std::atomic<bool> & running_;

    std::vector<std::function<bool(void)>> checks_;
};

} // namespace wait
} // namespace policy
} // namespace marklar_dataflow
