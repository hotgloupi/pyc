#pragma once

#include <pyc/parser/fwd.hpp>
#include <pyc/parser/SourceLocation.hpp>

#include <vector>

namespace pyc { namespace parser {

    class Lexer
    {
    public:
        enum class Mode
        {
            file,
            single,
            eval,
        };
        typedef std::vector<std::pair<Token, SourceRange>> Stack;

    private:
        Mode const _mode;
        Source& _source;
        SourceLocation _current;

    public:
        Lexer(Source& source, Mode mode);

        Mode mode() const { return _mode; }

    public:
        bool parse(Stack& stack);
    };

}}
