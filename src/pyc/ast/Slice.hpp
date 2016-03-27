#pragma once

#include <pyc/ast/Expression.hpp>

namespace pyc { namespace ast {

    class Slice : public Expression
    {
    public:
        Ptr<Expression> from;
        Ptr<Expression> to;
        Ptr<Expression> step;

    public:
        explicit Slice(Ptr<Expression> from,
                       Ptr<Expression> to,
                       Ptr<Expression> step)
          : from(std::move(from))
          , to(std::move(to))
          , step(std::move(step))
        {}

    public:
        void dump(std::ostream& out, unsigned indent = 0) const override;
    };

}}


