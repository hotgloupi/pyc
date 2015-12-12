#pragma once

#include <iosfwd>

namespace pyc { namespace parser {

    enum class Token
    {
        eof,
        eol,
        def,
        identifier,
        string,
        number,
    };

    std::ostream& operator <<(std::ostream& out, Token tok);

}}
