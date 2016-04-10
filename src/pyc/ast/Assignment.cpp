#include "Assignment.hpp"

#include <iostream>

namespace pyc { namespace ast {

    void Assignment::dump(std::ostream& out, unsigned indent) const
    {
        out << "Assignment(\n";
        _indent(out, indent + 1) << "expression = ";
        _value->dump(out, indent + 1);
        out << "\n";
        _indent(out, indent) << ")";
    }

}}
