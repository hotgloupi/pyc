#pragma once

#include <iosfwd>

namespace pyc { namespace parser {
    enum class Token
    {
        newline,
        indent,
        dedent,
        identifier,
        keyword,
        literal,
        operator_,
        delimiter,
    };

    std::ostream& operator <<(std::ostream& out, Token tok);

}}
