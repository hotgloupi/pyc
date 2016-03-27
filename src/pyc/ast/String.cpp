#include "String.hpp"

#include <iostream>

namespace pyc { namespace ast {

    void String::dump(std::ostream& out, unsigned) const
    {
        out << '"' << this->value << '"';
    }

}}

