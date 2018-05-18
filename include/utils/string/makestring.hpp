#pragma once

#include <sstream>

#include <iostream>

namespace marklar_dataflow
{
namespace utils
{
namespace string
{

class MakeString
{
public:
    operator std::string() const
    {
        return stream_.str();
    }

    template<typename _paramT>
    MakeString & operator<< (_paramT const & data)
    {
        stream_ << data;

        return *this;
    }

    MakeString & precision(int particle_size)
    {
        stream_.precision(particle_size);

        return *this;
    }

    MakeString & flag(std::ios_base::fmtflags const & flag)
    {
        stream_.flags(flag);

        return *this;
    }

private:
    std::ostringstream stream_;
};

} // namespace string
} // namespace utils
} // namespace marklar_dataflow
