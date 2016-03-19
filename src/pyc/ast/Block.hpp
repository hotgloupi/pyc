#pragma once

#include <pyc/ast/Node.hpp>
#include <pyc/ast/Statement.hpp>

#include <vector>

namespace pyc { namespace ast {

    class Block : public Node
    {
    private:
        std::vector<Ptr<Statement>> _statements;

    public:
        void append(Ptr<Statement> stmt)
        { _statements.emplace_back(std::move(stmt)); }
    };

}}

