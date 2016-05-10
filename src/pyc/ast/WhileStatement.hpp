#pragma once

#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class WhileStatement : public Statement
    {
    public:
        WhileStatement()
          : Statement(NodeKind::while_statement)
        {}
    };

}}
