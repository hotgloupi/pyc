#pragma once

#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class WithStatement : public Statement
    {
    public:
        WithStatement()
          : Statement(NodeKind::with_statement)
        {}
    };

}}
