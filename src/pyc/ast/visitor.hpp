#pragma once

#include <pyc/ast/fwd.hpp>
#include <pyc/ast/NodeKind.hpp>

#include <type_traits>

namespace pyc { namespace ast {

    template<typename Base>
    class Visitor
    {
    public:
        template<typename N>
        void operator ()(N& node)
        { _dispatch(_self(), node); }

        template<typename N>
        void operator ()(N& node) const
        { _dispatch(_self(), node); }

    protected:
        Base& _self() { return static_cast<Base&>(*this); }
        Base const& _self() const { return static_cast<Base const&>(*this); }

        template<typename Self, typename N>
        static void _dispatch(Self& self, N& node)
        {
            if (std::is_final<N>::value)
                return self.visit(node);
            switch (node.kind)
            {
#define PYC_AST_DETAIL_NODE_CASE(kind, Type)                             \
    case kind:                                                           \
    {                                                                    \
        return self.visit(                                               \
          static_cast<typename std::conditional<                         \
            std::is_const<N>::value, NodeType const&, NodeType&>::type>( \
            node));                                                      \
    }
#include <pyc/ast/detail/nodes.hpp>
            }
        }
    };

}}

#define PYC_AST_VISIT_NODE(self, visitor)          \
    template <typename Visitor>                    \
    void visit(Visitor& visitor)                   \
    {                                              \
        visitor(*this);                            \
        _visit(*this, visitor);                    \
    }                                              \
    template <typename Visitor>                    \
    void visit(Visitor& visitor) const             \
    {                                              \
        visitor(*this);                            \
        _visit(*this, visitor);                    \
    }                                              \
    template <typename NodeType, typename Visitor> \
    void _visit(NodeType& self, Visitor& visitor)


