#include "BinaryExpression.hpp"

#include <iostream>

namespace pyc { namespace ast {

    void BinaryExpression::dump(std::ostream& out, unsigned indent) const
    {
        out << "BinaryExpression(\n";
        _indent(out, indent + 1) << "operator = " << this->token << ",\n";
        _indent(out, indent + 1) << "lhs = ";
        this->lhs->dump(out, indent + 1);
        out << ",\n";
        _indent(out, indent + 1) << "rhs = ";
        this->rhs->dump(out, indent + 1);
        out << "\n";
        _indent(out, indent) << ")";
    }

}}

