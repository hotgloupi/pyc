#pragma once

#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class ContinueStatement : public Statement
    {
    public:
        ContinueStatement()
          : Statement(NodeKind::continue_statement)
        {}
    };

}}

