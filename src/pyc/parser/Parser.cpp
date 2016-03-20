#include "Parser.hpp"
#include "SourceRange.hpp"
#include "Token.hpp"

#include <pyc/ast.hpp>
#include <pyc/str.hpp>

#include <cassert>
#include <iostream>
#include <unordered_map>

namespace pyc { namespace parser {

    std::unordered_map<Token, int> operator_precedence = {
        {Token::or_, 10},
        {Token::and_, 20},
        //{Token::not_, 30},

        {Token::less, 40},
        {Token::less_equal, 40},
        {Token::greater, 40},
        {Token::greater_equal, 40},
        {Token::equal_equal, 40},
        {Token::not_equal, 40},
        {Token::in, 40},
        {Token::not_in, 40},
        {Token::is, 40},
        {Token::is_not, 40},

        {Token::pipe, 50},
        {Token::circumflex, 60},
        {Token::ampersand, 70},
        {Token::left_shift, 80},
        {Token::right_shift, 80},
        {Token::plus, 90},
        {Token::minus, 90},
        {Token::star, 100},
        {Token::at, 100},
        {Token::slash, 100},
        {Token::percent, 100},
        {Token::double_slash, 100},
        {Token::double_star, 110}
    };

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

            std::string _str() const
            {
                if (_it == _end)
                    throw std::runtime_error("Unexpected end of stream");
                return std::string(_it->second.begin, _it->second.end);
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

            bool _eat_newlines()
            {
                bool found = false;
                while (_eat(Token::newline) || _eat(Token::semicolon))
                { found = true; }
                return found;
            }

            SourceRange _consume(Token token)
            {
                if (_tok() != token)
                    throw std::runtime_error("Expected " + str(token) +
                                             ", got " + str(_tok()));
                SourceRange res = _it->second;
                ++_it;
                return res;
            }

            // Parse top level statements
            Ptr<ast::Block> parse(Lexer::Mode mode)
            {
                return _block();
            }

            Ptr<ast::Block> _block()
            {
                auto block = make_unique<ast::Block>();

                while (true)
                {
                    _eat_newlines();
                    if (_tok() == Token::eof || _tok() == Token::dedent)
                        break;
                    block->statements.emplace_back(_stmt());
                }

                return block;
            }

            Ptr<ast::Block> _indented_block()
            {
                _eat_newlines();
                _consume(Token::indent);
                auto res = _block();
                if (_tok() != Token::eof)
                    _consume(Token::dedent);
                return res;
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
                {
                    if (_eat_newlines())
                        return make_unique<ast::ReturnStatement>();
                }
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
                {
                    auto fn = make_unique<ast::FunctionDefinition>();
                    auto name = _consume(Token::identifier);
                    fn->name = std::string(name.begin, name.end);
                    _consume(Token::left_parenthesis);
                    _consume(Token::right_parenthesis);
                    _consume(Token::colon);
                    fn->body = _indented_block();
                    return std::move(fn);
                }
                else if (_eat(Token::class_))
                    return make_unique<ast::ClassDefinition>();
                else if (_eat(Token::at))
                    return make_unique<ast::DecoratedDefinition>();
                else if (_eat(Token::async))
                    return make_unique<ast::AsyncDefinition>();
                throw std::runtime_error("Invalid token: " + str(_tok()) +
                                         " \"" + _str() + "\"");
            }

            Ptr<ast::Statement> _import_stmt()
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
