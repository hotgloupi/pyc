#pragma once

#include <pyc/parser/fwd.hpp>
#include <pyc/parser/SourceLocation.hpp>

namespace pyc { namespace parser {

    class Lexer
    {
    private:
        Source& _source;
        SourceLocation _current;

    public:
        Lexer(Source& source);

    public:
        bool next_token(Token& tok, SourceRange& location);
    };

}}
