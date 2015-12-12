#include "Lexer.hpp"
#include "Source.hpp"
#include "SourceRange.hpp"
#include "Token.hpp"

namespace pyc { namespace parser {

    Lexer::Lexer(Source& source)
      : _source(source)
      , _current(_source.begin())
    {}

    bool Lexer::next_token(Token& tok, SourceRange& range)
    {
restart:
        range.begin = _current;
        if (_current == _source.end())
            return false;

        range.end = _current;
        switch (*_current++)
        {
        case '\0':
            tok = Token::eof;
            return true;

        case ' ':
        case '\t':
        case '\f':
        case '\v':
            _current++;
            goto restart;
        case '\n':
            tok = Token::eol;
            return true;

        }

        return false;
    }

}}
