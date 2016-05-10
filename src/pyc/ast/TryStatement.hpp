#pragma once

#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class TryStatement : public Statement
    {
    public:
        TryStatement()
          : Statement(NodeKind::try_statement)
        {}
    };

}}
