#pragma once

#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class IfStatement : public Statement
    {
    public:
        IfStatement()
          : Statement(NodeKind::if_statement)
        {}
    };

}}
