#pragma once

#include <pyc/parser/Lexer.hpp>
#include <pyc/ast/Node.hpp>
#include <pyc/Ptr.hpp>

namespace pyc { namespace parser {

    class Parser
    {
    public:
        typedef Ptr<ast::Node> NodePtr;

    public:
        Parser();

    public:
        NodePtr parse(Lexer& lexer);
        NodePtr parse(Lexer::Stack const& stack);
    };

}}
