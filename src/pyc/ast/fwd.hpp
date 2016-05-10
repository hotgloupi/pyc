#pragma once

namespace pyc { namespace ast {

#define PYC_AST_DETAIL_NODE_CASE(kind, Type) class Type;
#include <pyc/ast/detail/nodes.hpp>

}}
