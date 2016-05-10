#include "NamedArgument.hpp"

#include <iostream>

namespace pyc { namespace ast {

    void NamedArgument::dump(std::ostream& out, unsigned indent) const
    {
        out << "NamedArgument(\n";
        _indent(out, indent + 1) << "name = \"" << this->name << "\",\n";
        _indent(out, indent + 1) << "value = ";
        this->value->dump(out, indent + 1);
        out << "\n";
        _indent(out, indent) << ")";
    }

}}
