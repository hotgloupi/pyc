#include "Block.hpp"

#include <iostream>

namespace pyc { namespace ast {

    void Block::dump(std::ostream& out, unsigned indent) const
    {
        out << "[\n";
        bool is_first = true;
        for (auto&& statement: this->statements)
        {
            if (is_first)
                is_first = false;
            else
                out << ",\n";
            _indent(out, indent + 1);
            statement->dump(out, indent + 1);
        }
        out << "\n";
        _indent(out, indent) << "]";
    }

}}
