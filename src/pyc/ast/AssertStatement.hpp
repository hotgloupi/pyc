#pragma once

#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class AssertStatement : public Statement
    {
    public:
        AssertStatement()
            : Statement(NodeKind::assert_statement)
        {}
    };

}}
