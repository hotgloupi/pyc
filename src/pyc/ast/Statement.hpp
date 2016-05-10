#pragma once

#include <pyc/ast/Node.hpp>

namespace pyc { namespace ast {

    class Statement : public Node
    {
    protected:
        using Node::Node;
    };

}}
