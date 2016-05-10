#pragma once

#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class PassStatement : public Statement
    {
    public:
        PassStatement()
          : Statement(NodeKind::pass_statement)
        {}
    };

}}


