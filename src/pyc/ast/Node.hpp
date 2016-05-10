#pragma once

#include <pyc/ast/fwd.hpp>
#include <pyc/ast/NodeKind.hpp>
#include <pyc/Ptr.hpp>

#include <iosfwd>

namespace pyc { namespace ast {

    class Node
    {
    public:
        NodeKind const kind;

    public:
        explicit Node(NodeKind kind)
          : kind(kind)
        {}
        virtual ~Node();

    public:
        virtual void dump(std::ostream& out, unsigned indent = 0) const;
    protected:
        std::ostream& _indent(std::ostream& out, unsigned indent = 0) const;
    };

}}
