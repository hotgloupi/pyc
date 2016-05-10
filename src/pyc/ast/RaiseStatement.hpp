#pragma once

#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class RaiseStatement : public Statement
    {
    public:
        RaiseStatement()
          : Statement(NodeKind::raise_statement)
        {}
    };

}}

