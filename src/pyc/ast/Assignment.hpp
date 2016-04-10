#pragma once

#include <pyc/ast/Statement.hpp>
#include <pyc/ast/BinaryExpression.hpp>

namespace pyc { namespace ast {

    class Assignment : public Statement
    {
    private:
        ConstPtr<BinaryExpression> _value;

    public:
        Assignment(Ptr<BinaryExpression> value)
            : _value(std::move(value))
        {}
        void dump(std::ostream& out, unsigned indent = 0) const override;
    };

}}
