#pragma once

#include <pyc/ast/fwd.hpp>
#include <pyc/Ptr.hpp>

#include <iosfwd>

namespace pyc { namespace ast {

    class Node
    {
    public:
        virtual ~Node();

    public:
        void dump(std::ostream& out);
    };

}}
