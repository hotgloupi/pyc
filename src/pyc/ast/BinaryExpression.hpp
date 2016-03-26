#pragma once

#include <pyc/ast/Expression.hpp>
#include <pyc/parser/Token.hpp>

namespace pyc { namespace ast {

    class BinaryExpression : public Expression
    {
    public:
        parser::Token const token;
        ConstPtr<Expression> lhs;
        ConstPtr<Expression> rhs;

    public:
        BinaryExpression(parser::Token token,
                         Ptr<Expression> lhs,
                         Ptr<Expression> rhs)
          : token(token)
          , lhs(std::move(lhs))
          , rhs(std::move(rhs))
        {}
    public:
        void dump(std::ostream& out, unsigned indent = 0) const override;
    };
}}
