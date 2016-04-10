#pragma once

#include <pyc/parser/SourceLocation.hpp>

#include <iosfwd>

namespace pyc { namespace parser {

    struct SourceRange
    {
        SourceLocation begin;
        SourceLocation end;
    };

    std::ostream& operator <<(std::ostream& out, SourceRange const& range);

}}
