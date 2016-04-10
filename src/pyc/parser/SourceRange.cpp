#include "SourceRange.hpp"

#include <iostream>

namespace pyc { namespace parser {

    std::ostream& operator <<(std::ostream& out, SourceRange const& range)
    {
        return out << std::string(range.begin, range.end);
    }
}}
