#include "Token.hpp"

#include <iostream>

namespace pyc { namespace parser {

    std::ostream& operator <<(std::ostream& out, Token tok)
    {
        switch (tok)
        {
#define CASE(name) case Token::name: return out << #name
        CASE(eof);
        CASE(identifier);
        CASE(number);
        CASE(string);
        CASE(newline);
        CASE(indent);
        CASE(dedent);
        CASE(left_parenthesis);
        CASE(right_parenthesis);
        CASE(left_square_bracket);
        CASE(right_square_bracket);
        CASE(colon);
        CASE(comma);
        CASE(semicolon);
        CASE(plus);
        CASE(minus);
        CASE(star);
        CASE(slash);
        CASE(pipe);
        CASE(ampersand);
        CASE(less);
        CASE(greater);
        CASE(equal);
        CASE(dot);
        CASE(percent);
        CASE(left_brace);
        CASE(right_brace);
        CASE(equal_equal);
        CASE(not_equal);
        CASE(less_equal);
        CASE(greater_equal);
        CASE(tilde);
        CASE(circumflex);
        CASE(left_shift);
        CASE(right_shift);
        CASE(double_star);
        CASE(plus_equal);
        CASE(minus_equal);
        CASE(star_equal);
        CASE(slash_equal);
        CASE(percent_equal);
        CASE(ampersand_equal);
        CASE(pipe_equal);
        CASE(circumflex_equal);
        CASE(left_shift_equal);
        CASE(right_shift_equal);
        CASE(double_star_equal);
        CASE(double_slash);
        CASE(double_slash_equal);
        CASE(at);
        CASE(at_equal);
        CASE(right_arrow);
        CASE(ellipsis);
        CASE(op);
        }
        std::abort();
    }
}}
