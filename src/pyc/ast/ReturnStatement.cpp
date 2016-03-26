#include "ReturnStatement.hpp"

#include <iostream>

namespace pyc { namespace ast {

    void ReturnStatement::dump(std::ostream& out, unsigned indent) const
    {
        out << "{\n";
        _indent(out, indent + 1) << "class: " << "ReturnStatement,\n";
        _indent(out, indent + 1) << "value: ";
        this->value->dump(out, indent + 1);
        out << "\n";
        _indent(out, indent) << "}";
    }

}}
