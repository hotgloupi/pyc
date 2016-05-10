#pragma once

#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class BreakStatement : public Statement
    {
    public:
        BreakStatement()
          : Statement(NodeKind::break_statement)
        {}
    };

}}
