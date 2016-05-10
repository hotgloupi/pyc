#pragma once

#include <pyc/ast/fwd.hpp>

namespace pyc { namespace ast {

    enum class NodeKind
    {
#define PYC_AST_DETAIL_NODE_CASE(kind, Type) kind,
#include <pyc/ast/detail/nodes.hpp>
    };

    template<NodeKind kind> struct from_kind;
    template<typename NodeType> struct to_kind;

#define PYC_AST_DETAIL_NODE_CASE(kind, Type)              \
    template <>                                           \
    struct from_kind<NodeKind::kind>                      \
    {                                                     \
        typedef Type type;                                \
    };                                                    \
    template <>                                           \
    struct to_kind<Type>                                  \
    {                                                     \
        static NodeKind constexpr value = NodeKind::kind; \
    };
#include <pyc/ast/detail/nodes.hpp>

}}
