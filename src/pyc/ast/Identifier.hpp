#pragma once

#include <pyc/ast/Expression.hpp>

#include <string>

namespace pyc { namespace ast {

    class Identifier : public Expression
    {
    public:
        std::string const value;

    public:
        explicit Identifier(std::string value)
          : Expression(NodeKind::identifier)
          , value(std::move(value))
        {}

    public:
        void dump(std::ostream& out, unsigned indent = 0) const override;
    };

}}
