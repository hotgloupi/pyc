#pragma once

#include <iosfwd>
#include <functional>

namespace pyc { namespace parser {
    enum class Token
    {
        eof,
        identifier,

        _keyword_begin,
        as = _keyword_begin,
        async,
        break_,
        continue_,
        def,
        del,
        from,
        import,
        pass,
        raise,
        return_,
        global,
        nonlocal,
        assert_,
        if_,
        elif,
        else_,
        while_,
        for_,
        try_,
        finally,
        with,
        except,
        lambda,
        or_,
        and_,
        not_,
        await,
        class_,
        yield,
        _keyword_end,

        number = _keyword_end,
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
        in,
        not_in,
        is,
        is_not,

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

namespace std {

    template <>
    struct hash<pyc::parser::Token>
      : public hash<underlying_type<pyc::parser::Token>::type>
    {
        typedef underlying_type<pyc::parser::Token>::type Underlying;
        typedef hash<Underlying> Super;

        size_t operator()(pyc::parser::Token token) const
        {
            return Super::operator()(static_cast<Underlying>(token));
        }
    };

}
