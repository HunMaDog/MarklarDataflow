#pragma once

// Marklar headers
#include <attribute.hpp>
#include <command/command.hpp>
#include <command/dashboard.hpp>
// 3rd party library headers
// C++ standard Library headers

// Forward declarations
// Classes and structs
namespace marklar_dataflow
{
namespace command
{

class Passthrough
    : public Command
{
protected:
    void command(Dashboard * dashboard) final
    {
        dashboard->passthrough();
    }
};

static constexpr char passthrough[] = "passthrough";

} // namespace command
} // namespace marklar_dataflow
