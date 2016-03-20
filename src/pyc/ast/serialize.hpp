#pragma once

#include <pyc/ast/Node.hpp>

#include <iosfwd>

namespace pyc { namespace ast { namespace serialize {

    void to_xml(std::ostream& out, Node const& node);
    void to_text(std::ostream& out, Node const& node);

}}}
