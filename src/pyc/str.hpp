#pragma once

#include <boost/lexical_cast.hpp>

#include <string>

namespace pyc {

    template <typename T>
    std::string str(T const& value)
    {
        return boost::lexical_cast<std::string>(value);
    }

}
