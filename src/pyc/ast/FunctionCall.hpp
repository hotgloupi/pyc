#pragma once

#include <pyc/ast/Expression.hpp>

namespace pyc { namespace ast {

    class FunctionCall : public Expression
    {
    public:
        Ptr<Expression> expression;
        Ptr<Expression> args;

    public:
        FunctionCall(Ptr<Expression> callable, Ptr<Expression> args)
          : expression(std::move(callable))
          , args(std::move(args))
        {}

    public:
        void dump(std::ostream& out, unsigned indent = 0) const override;
    };

}}
