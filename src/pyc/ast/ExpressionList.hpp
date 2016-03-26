#pragma once

#include <pyc/ast/Expression.hpp>

#include <vector>

namespace pyc { namespace ast {

    class ExpressionList : public Expression
    {
    public:
        std::vector<Ptr<Expression>> values;
    };

}}

