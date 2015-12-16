#include "Token.hpp"

#include <iostream>

namespace pyc { namespace parser {

    std::ostream& operator <<(std::ostream& out, Token tok)
    {
        switch (tok)
        {
#define CASE(name) case Token::name: return out << "Token::" #name;
        CASE(newline);
        CASE(indent);
        CASE(dedent);
        CASE(identifier);
        CASE(keyword);
        CASE(literal);
        CASE(operator_);
        CASE(delimiter);
        }
        std::abort();
    }
}}
