#include "Parser.hpp"
#include "SourceRange.hpp"
#include "Token.hpp"

#include <pyc/ast.hpp>

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
            Parser::NodePtr parse(Lexer::Mode mode)
            {
                _stmt();

                return nullptr;
            }

            // stmt: simple_stmt | compound_stmt
            // simple_stmt: small_stmt (';' small_stmt)* [';'] NEWLINE
            // small_stmt: (expr_stmt | del_stmt | pass_stmt | flow_stmt |
            //             import_stmt | global_stmt | nonlocal_stmt | assert_stmt)
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
                else if (_eat(Token::assert))
                    return make_unique<ast::AssertStatement>();
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
