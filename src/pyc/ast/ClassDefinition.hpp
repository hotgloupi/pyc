#pragma once

#include <pyc/ast/Statement.hpp>

namespace pyc { namespace ast {

    class ClassDefinition : public Statement
    {
    public:
        ClassDefinition()
          : Statement(NodeKind::class_definition)
        {}
    };

}}
