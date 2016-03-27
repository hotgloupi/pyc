#include "Slice.hpp"

#include <iostream>

namespace pyc { namespace ast {

    void Slice::dump(std::ostream& out, unsigned indent) const
    {
        out << "{\n";
        _indent(out, indent + 1) << "class: " << "Slice,\n";

        _indent(out, indent + 1) << "from: ";
        this->from->dump(out, indent + 1);
        out << ",\n";

        _indent(out, indent + 1) << "to: ";
        this->to->dump(out, indent + 1);
        out << ",\n";

        _indent(out, indent + 1) << "step: ";
        this->step->dump(out, indent + 1);
        out << "\n";

        _indent(out, indent) << "}";
    }

}}

