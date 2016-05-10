#pragma once

#include <pyc/ast/Expression.hpp>

namespace pyc { namespace ast {

    class NamedArgument : public Expression
    {
    public:
        std::string const name;
        ConstPtr<Expression> value;

    public:
        NamedArgument(std::string name, Ptr<Expression> value)
          : Expression(NodeKind::named_argument)
          , name(std::move(name))
          , value(std::move(value))
        {}

    public:
        void dump(std::ostream& out, unsigned indent = 0) const override;
    };

}}
