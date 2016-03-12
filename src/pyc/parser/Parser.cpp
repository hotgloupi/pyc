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

            bool read(Token token)
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

                switch (_tok())
                {
#define CASE(token, AstType, ...) \
    case Token::token:            \
        read(Token::token);       \
        return make_unique<ast::AstType>(__VA_ARGS__)
                    CASE(del, DelStatement, _exprlist());
                    CASE(pass, PassStatement);
                    CASE(break_, BreakStatement);
                    CASE(continue_, ContinueStatement);
                    CASE(return_, ReturnStatement);
                    CASE(raise, RaiseStatement);
                    CASE(yield, YieldStatement);
                default:
                    return nullptr;
                }
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
