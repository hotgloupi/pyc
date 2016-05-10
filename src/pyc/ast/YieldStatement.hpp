#pragma once

#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class YieldStatement : public Statement
    {
    public:
        YieldStatement()
          : Statement(NodeKind::yield_statement)
        {}
    };

}}
