#pragma once

#include <pyc/ast/Expression.hpp>

#include <vector>

namespace pyc { namespace ast {

    class ExpressionList : public Expression
    {
    public:
        std::vector<Ptr<Expression>> values;

    public:
        ExpressionList()
          : Expression(NodeKind::expression_list)
        {}

    public:
        void dump(std::ostream& out, unsigned indent = 0) const override;
    };

}}

