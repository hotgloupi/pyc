#include "Parser.hpp"
#include "SourceRange.hpp"
#include "Token.hpp"

#include <pyc/ast.hpp>
#include <pyc/str.hpp>

#include <cassert>
#include <iostream>

namespace pyc { namespace parser {

    namespace {

        struct Context
        {
            Lexer::Stack::const_iterator _it;
            Lexer::Stack::const_iterator _end;

            Context(Lexer::Stack const& stack)
                : _it(stack.begin())
                , _end(stack.end())
            {
                if (_it == _end)
                    throw std::runtime_error("Empty stack");
            }

            Token _tok() const
            {
                if (_it == _end)
                    throw std::runtime_error("Unexpected end of stream");
                return _it->first;
            }

            bool _eat(Token token)
            {
                if (_tok() == token)
                {
                    ++_it;
                    return true;
                }
                return false;
            }

            // Parse top level statements
            Ptr<ast::Block> parse(Lexer::Mode mode)
            {
                auto block = make_unique<ast::Block>();

                while (true)
                {
                    while (_eat(Token::newline) || _eat(Token::semicolon))
                    {  }
                    if (_eat(Token::eof))
                        break;
                    block->append(_stmt());
                }

                return block;
            }

            // stmt: simple_stmt | compound_stmt
            // simple_stmt: small_stmt (';' small_stmt)* [';'] NEWLINE
            // small_stmt: (expr_stmt | del_stmt | pass_stmt | flow_stmt |
            //             import_stmt | global_stmt | nonlocal_stmt | assert_stmt)
            // compound_stmt: if_stmt | while_stmt | for_stmt | try_stmt |
            //                with_stmt | funcdef | classdef | decorated | async_stmt
            Ptr<ast::Statement> _stmt()
            {
                if (_eat(Token::del))
                    return make_unique<ast::DelStatement>(_exprlist());
                else if (_eat(Token::pass))
                    return make_unique<ast::PassStatement>();
                else if (_eat(Token::break_))
                    return make_unique<ast::BreakStatement>();
                else if (_eat(Token::continue_))
                    return make_unique<ast::ContinueStatement>();
                else if (_eat(Token::return_))
                    return make_unique<ast::ReturnStatement>();
                else if (_eat(Token::raise))
                    return make_unique<ast::RaiseStatement>();
                else if (_eat(Token::yield))
                    return make_unique<ast::YieldStatement>();
                else if (_tok() == Token::import || _tok() == Token::from)
                    return _import_stmt();
                else if (_eat(Token::global))
                    return make_unique<ast::GlobalStatement>();
                else if (_eat(Token::nonlocal))
                    return make_unique<ast::NonLocalStatement>();
                else if (_eat(Token::assert_))
                    return make_unique<ast::AssertStatement>();
                else if (_eat(Token::if_))
                    return make_unique<ast::IfStatement>();
                else if (_eat(Token::while_))
                    return make_unique<ast::WhileStatement>();
                else if (_eat(Token::for_))
                    return make_unique<ast::ForStatement>();
                else if (_eat(Token::try_))
                    return make_unique<ast::TryStatement>();
                else if (_eat(Token::with))
                    return make_unique<ast::WithStatement>();
                else if (_eat(Token::def))
                    return make_unique<ast::FunctionDefinition>();
                else if (_eat(Token::class_))
                    return make_unique<ast::ClassDefinition>();
                else if (_eat(Token::at))
                    return make_unique<ast::DecoratedDefinition>();
                else if (_eat(Token::async))
                    return make_unique<ast::AsyncDefinition>();
                throw std::runtime_error("Invalid token: " + str(_tok()));
            }

            Ptr<ast::Statement> _import_stmt()
            {
                return nullptr;
            }

            Ptr<ast::Statement> _compound_stmt()
            {
                return nullptr;
            }

            Ptr<ast::ExpressionList> _exprlist()
            {
                return nullptr;
            }
        };

    }

    Parser::Parser()
    {}

    Parser::NodePtr Parser::parse(Lexer& lexer)
    {
        parser::Lexer::Stack stack;
        if (lexer.parse(stack))
        {
            Context context(stack);
            return context.parse(lexer.mode());
        }
        return nullptr;
    }

}}
