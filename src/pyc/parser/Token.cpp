#include "Token.hpp"

#include <iostream>

namespace pyc { namespace parser {

    std::ostream& operator <<(std::ostream& out, Token tok)
    {
        switch (tok)
        {
#define CASE(name) case Token::name: return out << "Token::" #name;
        CASE(eof);
        CASE(eol);
        CASE(def);
        CASE(identifier);
        CASE(string);
        CASE(number);
        }
        std::abort();
    }
}}
