#include <policy/scheduler/checklist.hpp>

// Marklar headers
#include <error/exception.hpp>
// 3rd party library headers
// C++ standard Library headers

using marklar_dataflow::policy::scheduler::Checklist;

Checklist::Checklist(std::atomic<bool> & running)
    : running_(running)
{}

void Checklist::push_check(std::function<bool(void)> && checkFunction)
{
    checks_.emplace_back(std::move(checkFunction));
}

void Checklist::preparation()
{
    if(checks_.empty())
        throw error::Exception("empty wait checks", 0);
}

void Checklist::termination()
{}

bool Checklist::is_ready()
{
    for(const auto& checking : checks_)
        if(!checking() && running_)
            return false;

    return running_;
}
