#pragma once

#include <pyc/ast/Expression.hpp>
#include <pyc/parser/Token.hpp>

namespace pyc { namespace ast {

    class UnaryExpression : public Expression
    {
    public:
        parser::Token const token;
        ConstPtr<Expression> expression;

    public:
        UnaryExpression(parser::Token token,
                        Ptr<Expression> expression)
          : token(token)
          , expression(std::move(expression))
        {}
    };
}}
