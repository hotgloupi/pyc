#pragma once

#include <pyc/ast/Node.hpp>
#include <pyc/ast/Statement.hpp>

#include <vector>

namespace pyc { namespace ast {

    class Block : public Node
    {
    public:
        std::vector<Ptr<Statement>> statements;
    };

}}
