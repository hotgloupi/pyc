#pragma once

#include <pyc/ast/Statement.hpp>
#include <pyc/ast/Block.hpp>

#include <string>

namespace pyc { namespace ast {

    class FunctionDefinition : public Statement
    {
    public:
        std::string name;
        Ptr<Block> body;
    };

}}
