#pragma once

#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class GlobalStatement : public Statement
    {
    public:
        GlobalStatement()
          : Statement(NodeKind::global_statement)
        {}
    };

}}
