#pragma once

#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class DecoratedDefinition : public Statement
    {
    public:
        DecoratedDefinition()
          : Statement(NodeKind::decorated_definition)
        {}
    };

}}
