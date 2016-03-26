#pragma once

#include <pyc/ast/Node.hpp>
#include <pyc/ast/Statement.hpp>

#include <vector>

namespace pyc { namespace ast {

    class Block : public Node
    {
    public:
        std::vector<Ptr<Statement>> statements;
    public:
        void dump(std::ostream& out, unsigned indent = 0) const override;
    };

}}
