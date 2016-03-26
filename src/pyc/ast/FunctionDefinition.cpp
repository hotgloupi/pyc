#include "FunctionDefinition.hpp"

#include <iostream>

namespace pyc { namespace ast {

    void FunctionDefinition::dump(std::ostream& out, unsigned indent) const
    {
        out << "{\n";
        _indent(out, indent + 1) << "class: " << "FunctionDefinition,\n";
        _indent(out, indent + 1) << "name: " << this->name << ",\n";
        _indent(out, indent + 1) << "body: ";
        this->body->dump(out, indent + 1);
        out << "\n";
        _indent(out, indent) << "}";
    }

}}
