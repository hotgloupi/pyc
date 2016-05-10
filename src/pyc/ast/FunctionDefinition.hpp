#pragma once

#include <pyc/ast/Statement.hpp>
#include <pyc/ast/Block.hpp>
#include <pyc/ast/ExpressionList.hpp>

#include <string>

namespace pyc { namespace ast {

    class FunctionDefinition : public Statement
    {
    public:
        std::string const name;
        ConstPtr<ExpressionList> args;
        ConstPtr<Block> body;

    public:
        FunctionDefinition(std::string name,
                           Ptr<ExpressionList> args,
                           Ptr<Block> body)
          : Statement(NodeKind::function_definition)
          , name(std::move(name))
          , args(std::move(args))
          , body(std::move(body))
        {}

    public:
        void dump(std::ostream& out, unsigned indent = 0) const override;
    };

}}
