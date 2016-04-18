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

#define NOT_IMPLEMENTED()                                                 \
    throw std::runtime_error("Not implemented " + std::string(__FILE__) + \
                             ":" + std::to_string(__LINE__));

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

            Token _eat()
            {
                if (_it == _end)
                    throw std::runtime_error("Unexpected end of stream");
                auto tok = _tok();
                ++_it;
                return tok;
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
            Ptr<ast::Block> parse(Lexer::Mode /*mode*/)
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
                if (!_eat_newlines())
                {
                    // One line block
                    auto block = make_unique<ast::Block>();
                    block->statements.emplace_back(_stmt());
                    return block;
                }
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
            // return_stmt: 'return' [testlist]
            Ptr<ast::Statement> _stmt()
            {
                if (_eat(Token::del))
                    return make_unique<ast::DelStatement>(_expression_list());
                else if (_eat(Token::pass))
                    return make_unique<ast::PassStatement>();
                else if (_eat(Token::break_))
                    return make_unique<ast::BreakStatement>();
                else if (_eat(Token::continue_))
                    return make_unique<ast::ContinueStatement>();
                else if (_eat(Token::return_))
                {
                    auto res = make_unique<ast::ReturnStatement>();
                    if (!_eat_newlines())
                        res->value = _expression_list();
                    return std::move(res);
                }
                else if (_eat(Token::raise))
                {
                    NOT_IMPLEMENTED();
                    return make_unique<ast::RaiseStatement>();
                }
                else if (_eat(Token::yield))
                {
                    NOT_IMPLEMENTED();
                    return make_unique<ast::YieldStatement>();
                }
                else if (_tok() == Token::import || _tok() == Token::from)
                    return _import_stmt();
                else if (_eat(Token::global))
                {
                    NOT_IMPLEMENTED();
                    return make_unique<ast::GlobalStatement>();
                }
                else if (_eat(Token::nonlocal))
                {
                    NOT_IMPLEMENTED();
                    return make_unique<ast::NonLocalStatement>();
                }
                else if (_eat(Token::assert_))
                {
                    NOT_IMPLEMENTED();
                    return make_unique<ast::AssertStatement>();
                }
                else if (_eat(Token::if_))
                {
                    NOT_IMPLEMENTED();
                    return make_unique<ast::IfStatement>();
                }
                else if (_eat(Token::while_))
                {
                    NOT_IMPLEMENTED();
                    return make_unique<ast::WhileStatement>();
                }
                else if (_eat(Token::for_))
                {
                    NOT_IMPLEMENTED();
                    return make_unique<ast::ForStatement>();
                }
                else if (_eat(Token::try_))
                {
                    NOT_IMPLEMENTED();
                    return make_unique<ast::TryStatement>();
                }
                else if (_eat(Token::with))
                {
                    NOT_IMPLEMENTED()
                    return make_unique<ast::WithStatement>();
                }
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
                {
                    NOT_IMPLEMENTED();
                    return make_unique<ast::ClassDefinition>();
                }
                else if (_eat(Token::at))
                {
                    NOT_IMPLEMENTED();
                    return make_unique<ast::DecoratedDefinition>();
                }
                else if (_eat(Token::async))
                {
                    NOT_IMPLEMENTED();
                    return make_unique<ast::AsyncDefinition>();
                }

                // Everything failed, we can try to read an "expr_stmt"
                // expr_stmt: testlist_star_expr (augassign (yield_expr|testlist) |
                //                      ('=' (yield_expr|testlist_star_expr))*)
                // testlist_star_expr: (test|star_expr) (',' (test|star_expr))* [',']
                // augassign: ('+=' | '-=' | '*=' | '@=' | '/=' | '%=' | '&=' |
                //             '|=' | '^=' | '<<=' | '>>=' | '**=' | '//=')

                auto lhs = _expression_list();
                if (_is_one_of(Token::newline, Token::semicolon))
                    return make_unique<ast::ExpressionStatement>(std::move(lhs));
                else if (_is_one_of(Token::equal,
                                    Token::plus_equal,
                                    Token::minus_equal,
                                    Token::star_equal,
                                    Token::at_equal,
                                    Token::slash_equal,
                                    Token::percent_equal,
                                    Token::ampersand_equal,
                                    Token::pipe_equal,
                                    Token::not_equal,
                                    Token::left_shift_equal,
                                    Token::right_shift_equal,
                                    Token::double_star_equal,
                                    Token::double_slash_equal
                                    ))
                {
                    auto token = _eat();
                    auto rhs = _expression_list();
                    return make_unique<ast::Assignment>(
                        make_unique<ast::BinaryExpression>(
                            token,
                            std::move(lhs),
                            std::move(rhs)
                        )
                    );
                }

                throw std::runtime_error("Invalid token: " + str(_tok()) +
                                         " \"" + _str() + "\"");
            }

            // import_stmt: import_name | import_from
            // import_name: 'import' dotted_as_names
            // import_from: ('from' (('.' | '...')* dotted_name | ('.' | '...')+)
            //               'import' ('*' | '(' import_as_names ')' | import_as_names))
            // import_as_name: NAME ['as' NAME]
            // dotted_as_name: dotted_name ['as' NAME]
            // import_as_names: import_as_name (',' import_as_name)* [',']
            // dotted_as_names: dotted_as_name (',' dotted_as_name)*
            // dotted_name: NAME ('.' NAME)*
            Ptr<ast::Statement> _import_stmt()
            {
                std::vector<std::string> from;
                typedef ast::ImportStatement::Style ImportStyle;
                ImportStyle style = ImportStyle::absolute;

                if (_eat(Token::from))
                {
                    if (_eat(Token::dot))
                    {
                        if (_eat(Token::dot))
                            style = ImportStyle::relative_to_parent;
                        else
                            style = ImportStyle::relative_to_current;
                    }
                    from = _dotted_name();
                }
                _consume(Token::import);
                std::vector<ast::ImportStatement::ImportAs> imports;
                while (true)
                {
                    ast::ImportStatement::ImportAs import_as;
                    import_as.import = _dotted_name();
                    if (_eat(Token::as))
                    {
                        import_as.as = _str();
                        _eat();
                    }
                    imports.emplace_back(std::move(import_as));
                    _eat(Token::comma);
                    if (_eat_newlines())
                        break;
                }
                return make_unique<ast::ImportStatement>(style,
                                                         std::move(from),
                                                         std::move(imports));
            }

            std::vector<std::string> _dotted_name()
            {
                std::vector<std::string> res;
                while (true)
                {
                    if (_eat(Token::dot))
                    {
                        continue;
                    }
                    else if (_tok() == Token::identifier)
                    {
                        res.emplace_back(_str());
                        _eat();
                    }
                    else
                        break;
                }
                return res;
            }

            template<typename... Tokens>
            bool _is_one_of(Tokens... tokens)
            {
                Token current = _tok();
                Token values[] = { static_cast<Token>(tokens)... };
                for (auto tok: values)
                    if (tok == current) return true;
                return false;
            }

            Ptr<ast::Slice> _slice()
            {
                Ptr<ast::Expression> from, to, step;
                from = _expression_atom();
                if (_eat(Token::colon))
                {
                    to = _expression_atom();
                    if (_eat(Token::colon))
                        step = _expression_atom();
                }
                return make_unique<ast::Slice>(
                  std::move(from), std::move(to), std::move(step));
            }

            Ptr<ast::Expression> _expression_atom()
            {

                if (_is_one_of(Token::plus, Token::minus, Token::tilde))
                {
                    //return make_unique<ast::UnaryExpression>(_eat(), )
                    NOT_IMPLEMENTED();
                }

                Ptr<ast::Expression> res;
                if (_eat(Token::left_parenthesis))
                {
                    res = _expression_list();
                    _eat_newlines();
                    _consume(Token::right_parenthesis);
                }
                else if (_eat(Token::lambda))
                {
                    // XXX parse lambda
                    NOT_IMPLEMENTED();
                }
                else if (_tok() == Token::identifier)
                {
                    res = make_unique<ast::Identifier>(_str());
                    _eat();
                }
                else if (_tok() == Token::unicode)
                {
                    res = make_unique<ast::String>(_str());
                    _eat();
                }
                else if (_tok() == Token::raw_unicode)
                {
                    NOT_IMPLEMENTED();
                }
                else if (_tok() == Token::bytes)
                {
                    NOT_IMPLEMENTED();
                }
                else if (_tok() == Token::raw_bytes)
                {
                    NOT_IMPLEMENTED();
                }
                else if (_tok() == Token::formatted_string)
                {
                    NOT_IMPLEMENTED();
                }
                else if (_tok() == Token::raw_formatted_string)
                {
                    NOT_IMPLEMENTED();
                }
                else if (_tok() == Token::number)
                {
                    res = make_unique<ast::Number>(_str());
                    _eat();
                }
                else if (_tok() == Token::ellipsis)
                {
                    NOT_IMPLEMENTED();
                }
                else
                    NOT_IMPLEMENTED();

                while (true)
                {
                    if (_eat(Token::left_parenthesis))
                    {
                        auto args = _args();
                        _consume(Token::right_parenthesis);
                        res = make_unique<ast::FunctionCall>(
                          std::move(res), std::move(args));
                    }
                    else if (_eat(Token::left_square_bracket))
                    {
                        auto slice = _slice();
                        _consume(Token::right_square_bracket);
                        res = make_unique<ast::GetItem>(
                          std::move(res), std::move(slice));
                    }
                    else if (_eat(Token::dot))
                    {
                        NOT_IMPLEMENTED();
                    }
                    else
                        break;
                }
                return res;
            }


            Ptr<ast::ExpressionList> _args()
            {
                auto res = make_unique<ast::ExpressionList>();
                while (_tok() != Token::right_parenthesis)
                {
                    res->values.emplace_back(_expression_atom());
                    if (_eat(Token::equal))
                    {
                        NOT_IMPLEMENTED();
                    }
                    if (!_eat(Token::comma))
                        break;
                }
                return res;
            }

            // The operator precedence or 0
            int _precedence(Token op)
            {
                if (_it == _end) return 0;
                auto it = operator_precedence.find(op);
                if (it == operator_precedence.end()) return 0;
                return it->second;
            }

            Ptr<ast::Expression> _expression(int last_precedence = -1,
                                             Ptr<ast::Expression> lhs = nullptr)
            {
                if (lhs == nullptr)
                    lhs = _expression_atom();

                while (true)
                {
                    auto precedence = _precedence(_tok());
                    if (precedence == 0)
                    {
                        if (_eat(Token::for_))
                        {
                            // XXX Comprehension list here
                            NOT_IMPLEMENTED();
                        }
                        return lhs;
                    }
                    if (precedence < last_precedence)
                        return lhs;

                    auto op = _eat();
                    auto rhs = _expression_atom();
                    auto next_precedence = _precedence(_tok());
                    if (next_precedence > precedence)
                    {
                        rhs = _expression(precedence + 1, std::move(rhs));
                    }
                    lhs = make_unique<ast::BinaryExpression>(
                        op,
                        std::move(lhs),
                        std::move(rhs)
                    );
                }

            }

            // Parse an expression or a list of expression
            // Covers testlist and exprlist
            Ptr<ast::Expression> _expression_list()
            {
                Ptr<ast::ExpressionList> list = nullptr;
                Ptr<ast::Expression> current_expression = nullptr;
                while (true)
                {
                    if (_tok() == Token::newline || _tok() == Token::eof)
                    {
                        break;
                    }
                    else
                    {
                        current_expression = _expression();
                    }
                    assert(current_expression != nullptr);

                    if (_eat(Token::comma))
                    {
                        if (list == nullptr)
                            list = make_unique<ast::ExpressionList>();
                        if (current_expression != nullptr)
                            list->values.emplace_back(
                              std::move(current_expression));
                    }
                    else
                    {
                        if (list == nullptr)
                            return current_expression;
                        if (current_expression != nullptr)
                            list->values.emplace_back(
                              std::move(current_expression));
                        return std::move(list);
                    }
                }
                return std::move(list);
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
