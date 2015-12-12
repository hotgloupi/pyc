#pragma once

#include <pyc/parser/SourceLocation.hpp>

namespace pyc { namespace parser {

    struct SourceRange
    {
        SourceLocation begin;
        SourceLocation end;
    };

}}
