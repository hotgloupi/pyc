#include "Identifier.hpp"

#include <iostream>

namespace pyc { namespace ast {

    void Identifier::dump(std::ostream& out, unsigned) const
    {
        out << '$' << this->value;
    }

}}
