#pragma once

#include <pyc/ast/Expression.hpp>

namespace pyc { namespace ast {

    class String : public Expression
    {
    public:
        std::string const value;

    public:
        explicit String(std::string value)
          : value(std::move(value))
        {}
    public:
        void dump(std::ostream& out, unsigned indent = 0) const override;
    };

}}

