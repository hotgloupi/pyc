#pragma once

#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class ForStatement : public Statement
    {
    public:
        ForStatement()
          : Statement(NodeKind::for_statement)
        {}
    };

}}
