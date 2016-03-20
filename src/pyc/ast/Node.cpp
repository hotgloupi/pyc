#include "Node.hpp"
#include "serialize.hpp"

namespace pyc { namespace ast {

    Node::~Node()
    {}

    void Node::dump(std::ostream& out)
    {
        serialize::to_xml(out, *this);
    }

}}
