#include "Number.hpp"

#include <iostream>

namespace pyc { namespace ast {

    void Number::dump(std::ostream& out, unsigned) const
    {
        out << this->value;
    }

}}

