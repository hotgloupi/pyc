#include "ExpressionList.hpp"

#include <iostream>

namespace pyc { namespace ast {

    void ExpressionList::dump(std::ostream& out, unsigned indent) const
    {
        out << "[";
        bool first = true;
        for (auto&& ptr: this->values)
        {
            if (first) first = false;
            else out << ",";
            out << "\n";
            _indent(out, indent + 1);
            ptr->dump(out, indent + 1);
        }
        if (first)
            out << "]";
        else
        {
            out << "\n";
            _indent(out, indent) << "]";
        }
    }

}}

