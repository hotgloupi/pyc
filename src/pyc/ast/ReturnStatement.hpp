#pragma once

#include <pyc/ast/Expression.hpp>
#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class ReturnStatement : public Statement
    {
    public:
        Ptr<Expression> value;

    public:
        ReturnStatement()
          : Statement(NodeKind::return_statement)
        {}

    public:
        void dump(std::ostream& out, unsigned indent = 0) const override;
    };

}}
