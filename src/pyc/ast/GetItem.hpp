#pragma once

#include <pyc/ast/Slice.hpp>

namespace pyc { namespace ast {

    class GetItem : public Expression
    {
    public:
        Ptr<Expression> expression;
        Ptr<Slice> slice;

    public:
        explicit GetItem(Ptr<Expression> expression, Ptr<Slice> slice)
          : expression(std::move(expression))
          , slice(std::move(slice))
        {}

    public:
        void dump(std::ostream& out, unsigned indent = 0) const override;
    };

}}

