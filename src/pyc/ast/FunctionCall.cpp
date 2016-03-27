#include "FunctionCall.hpp"

#include <iostream>

namespace pyc { namespace ast {

    void FunctionCall::dump(std::ostream& out, unsigned indent) const
    {
        out << "{\n";
        _indent(out, indent + 1) << "class: " << "FunctionCall,\n";
        _indent(out, indent + 1) << "expression: ";
        this->expression->dump(out, indent + 1);
        out << ",\n";
        _indent(out, indent + 1) << "args: ";
        this->args->dump(out, indent + 1);
        out << "\n";
        _indent(out, indent) << "}";
    }

}}
