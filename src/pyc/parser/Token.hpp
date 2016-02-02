#pragma once

#include <iosfwd>

namespace pyc { namespace parser {
    enum class Token
    {
        eof,
        identifier,
        name = identifier,
        number,
        string,
        newline,
        indent,
        dedent,
        lpar,
        left_parenthesis = lpar,
        rpar,
        right_parenthesis = rpar,
        lsqb,
        left_square_bracket = lsqb,
        rsqb,
        right_square_bracket = rsqb,
        left_brace,
        right_brace,

        colon,
        comma,
        semicolon,

        plus,
        minus,
        star,
        slash,
        pipe,
        ampersand,
        equal,
        dot,
        tilde,
        circumflex,
        left_shift,
        right_shift,
        double_slash,
        double_star,
        percent,
        op,

        less,
        greater,
        double_equal,
        equal_equal = double_equal,
        not_equal,
        less_equal,
        greater_equal,


        plus_equal,
        minus_equal,
        star_equal,
        slash_equal,
        percent_equal,
        ampersand_equal,
        pipe_equal,
        circumflex_equal,
        left_shift_equal,
        right_shift_equal,
        double_star_equal,
        double_slash_equal,
        at_equal,

        at,
        right_arrow,
        ellipsis,
    };

    std::ostream& operator <<(std::ostream& out, Token tok);

}}
