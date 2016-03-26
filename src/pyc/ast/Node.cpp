#include "Node.hpp"

#include <ostream>
#include <iomanip>

namespace pyc { namespace ast {

    Node::~Node()
    {}

    void Node::dump(std::ostream& out, unsigned indent) const
    {

        out << "<Node>";
    }

    std::ostream& Node::_indent(std::ostream& out, unsigned indent) const
    {
        return out << std::string(indent * 2, ' ');
    }
}}
