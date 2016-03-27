#include "GetItem.hpp"

#include <iostream>

namespace pyc { namespace ast {

    void GetItem::dump(std::ostream& out, unsigned indent) const
    {
        out << "{\n";
        _indent(out, indent + 1) << "class: " << "GetItem,\n";
        _indent(out, indent + 1) << "expression: ";
        this->expression->dump(out, indent + 1);
        out << ",\n";
        _indent(out, indent + 1) << "slice: ";
        this->slice->dump(out, indent + 1);
        out << "\n";
        _indent(out, indent) << "}";
    }

}}
