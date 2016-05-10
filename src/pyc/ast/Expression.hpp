#pragma once

#include <pyc/ast/Node.hpp>

namespace pyc { namespace ast {

    class Expression : public Node
    {
    public:
        using Node::Node;
    };

}}
