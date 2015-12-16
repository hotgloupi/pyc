#include "Lexer.hpp"
#include "Source.hpp"
#include "SourceRange.hpp"
#include "Token.hpp"

namespace pyc { namespace parser {

#define PARSER(name) bool parse_##name(Token& tok, SourceRange& range)
#define PARSER_DECL(name) bool parse_##name(Token& tok, SourceRange& range)
#define PARSE(name) parse_##name(tok, range)

#define TERMINAL_PARSER(name, token) PARSER(name)

    bool parse_string(Token&, SourceRange&, char const* str);
#define PARSE_STR(str) parse_string(tok, range, str)

#define TERMINAL_PARSER_STR(name, str, token) \
    TERMINAL_PARSER(name, token) { return PARSE_STR(str); }



    TERMINAL_PARSER(NEWLINE, newline) {
        return PARSE_STR("\r\n") || PARSE_STR("\r") || PARSE_STR("\n");
    }

    TERMINAL_PARSER(IDENTIFIER, identifier)
    {

    }

    TERMINAL_PARSER(INDENT, indent)
    {

    }

    TERMINAL_PARSER(DEDENT, dedent)
    {

    }

    TERMINAL_PARSER_STR(AT, "@", delimiter);
    TERMINAL_PARSER_STR(COLON, ":", delimiter);
    TERMINAL_PARSER_STR(COMMA, ",", delimiter);
    TERMINAL_PARSER_STR(EQ, "=", delimiter);
    TERMINAL_PARSER_STR(LPAR, "(", delimiter);
    TERMINAL_PARSER_STR(MUL, "*", delimiter);
    TERMINAL_PARSER_STR(POW, "**", delimiter);
    TERMINAL_PARSER_STR(RARROW, "->", delimiter);
    TERMINAL_PARSER_STR(RPAR, ")", delimiter);
    TERMINAL_PARSER_STR(SEMICOLON, ";", delimiter);

    TERMINAL_PARSER_STR(AS, "as", keyword);
    TERMINAL_PARSER_STR(ASYNC, "async", keyword);
    TERMINAL_PARSER_STR(BREAK, "break", keyword);
    TERMINAL_PARSER_STR(CONTINUE, "continue", keyword);
    TERMINAL_PARSER_STR(DEF, "def", keyword);
    TERMINAL_PARSER_STR(DEL, "del", keyword);
    TERMINAL_PARSER_STR(DOT, ".", keyword);
    TERMINAL_PARSER_STR(ELLIPSIS, "...", keyword);
    TERMINAL_PARSER_STR(FROM, "from", keyword);
    TERMINAL_PARSER_STR(IMPORT, "import", keyword);
    TERMINAL_PARSER_STR(PASS, "pass", keyword);
    TERMINAL_PARSER_STR(RAISE, "raise", keyword);
    TERMINAL_PARSER_STR(RETURN, "return", keyword);
    TERMINAL_PARSER_STR(GLOBAL, "global", keyword);
    TERMINAL_PARSER_STR(LOCAL, "local", keyword);
    TERMINAL_PARSER_STR(ASSERT, "assert", keyword);
    TERMINAL_PARSER_STR(IF, "if", keyword);
    TERMINAL_PARSER_STR(ELIF, "elif", keyword);
    TERMINAL_PARSER_STR(ELSE, "else", keyword);
    TERMINAL_PARSER_STR(WHILE, "while", keyword);
    TERMINAL_PARSER_STR(FOR, "for", keyword);
    TERMINAL_PARSER_STR(IN, "in", keyword);
    TERMINAL_PARSER_STR(TRY, "try", keyword);
    TERMINAL_PARSER_STR(FINALLY, "finally", keyword);
    TERMINAL_PARSER_STR(WITH, "with", keyword);
    TERMINAL_PARSER_STR(EXCEPT, "except", keyword);
    TERMINAL_PARSER_STR(LAMBDA, "lambda", keyword);
    TERMINAL_PARSER_STR(OR, "or", keyword);
    TERMINAL_PARSER_STR(AND, "and", keyword);
    TERMINAL_PARSER_STR(NOT, "not", keyword);

    TERMINAL_PARSER_STR(OP_OR, "|", operator_);
    TERMINAL_PARSER_STR(OP_XOR, "^", operator_);
    TERMINAL_PARSER_STR(OP_AND, "&", operator_);
    TERMINAL_PARSER_STR(OP_RSHIFT, ">>", operator_);
    TERMINAL_PARSER_STR(OP_LSHIFT, "<<", operator_);
    TERMINAL_PARSER_STR(OP_ADD, "+", operator_);
    TERMINAL_PARSER_STR(OP_SUB, "-", operator_);

    PARSER_DECL(arith_expr);
    PARSER_DECL(term);
    PARSER_DECL(shift_expr);
    PARSER_DECL(arglist);
    PARSER_DECL(assert_stmt);
    PARSER_DECL(async_funcdef);
    PARSER_DECL(async_stmt);
    PARSER_DECL(augassign);
    PARSER_DECL(break_stmt);
    PARSER_DECL(classdef);
    PARSER_DECL(compound_stmt);
    PARSER_DECL(continue_stmt);
    PARSER_DECL(decorated);
    PARSER_DECL(del_stmt);
    PARSER_DECL(dotted_as_names);
    PARSER_DECL(dotted_name);
    PARSER_DECL(exprlist);
    PARSER_DECL(expr_stmt);
    PARSER_DECL(flow_stmt);
    PARSER_DECL(for_stmt);
    PARSER_DECL(funcdef);
    PARSER_DECL(global_stmt);
    PARSER_DECL(if_stmt);
    PARSER_DECL(import_as_names);
    PARSER_DECL(import_from);
    PARSER_DECL(import_name);
    PARSER_DECL(import_stmt);
    PARSER_DECL(nonlocal_stmt);
    PARSER_DECL(parameters);
    PARSER_DECL(pass_stmt);
    PARSER_DECL(raise_stmt);
    PARSER_DECL(return_stmt);
    PARSER_DECL(simple_stmt);
    PARSER_DECL(small_stmt);
    PARSER_DECL(star_expr);
    PARSER_DECL(suite);
    PARSER_DECL(test);
    PARSER_DECL(testlist);
    PARSER_DECL(testlist_star_expr);
    PARSER_DECL(tfpdef);
    PARSER_DECL(try_stmt);
    PARSER_DECL(typedargslist);
    PARSER_DECL(vfpdef);
    PARSER_DECL(while_stmt);
    PARSER_DECL(with_stmt);
    PARSER_DECL(yield_expr);
    PARSER_DECL(yield_stmt);
    PARSER_DECL(except_clause);
    PARSER_DECL(with_item);
    PARSER_DECL(expr);
    PARSER_DECL(lambdef);
    PARSER_DECL(lambdef_nocond);
    PARSER_DECL(or_test);
    PARSER_DECL(and_test);
    PARSER_DECL(not_test);
    PARSER_DECL(comparison);
    PARSER_DECL(comp_op);
    PARSER_DECL(xor_expr);
    PARSER_DECL(and_expr);

    // decorator: '@' dotted_name [ '(' [arglist] ')' ] NEWLINE
    PARSER(decorator)
    {
        if (!(PARSE(AT) && PARSE(dotted_name)))
            return false;
        if (PARSE(LPAR))
        {
            PARSE(arglist);
            if (!PARSE(RPAR))
                return false;
        }
        return PARSE(NEWLINE);
    }

    // decorators: decorator+
    PARSER(decorators)
    {
        if (PARSE(decorator))
        {
            while (PARSE(decorator)) {}
            return true;
        }
        return false;
    }

    // decorated: decorators (classdef | funcdef | async_funcdef)
    PARSER(decorated)
    {
        return PARSE(decorators) &&
               (PARSE(classdef) || PARSE(funcdef) || PARSE(async_funcdef));
    }

    // async_funcdef: 'async' funcdef
    PARSER(async_funcdef)
    {
        return PARSE(ASYNC) && PARSE(funcdef);
    }

    // funcdef: 'def' NAME parameters ['->' test] ':' suite
    PARSER(funcdef)
    {
        if (PARSE(DEF) && PARSE(IDENTIFIER) && PARSE(parameters))
        {
            if (PARSE(RARROW) && !PARSE(test))
                return false;
            return PARSE(COLON) && PARSE(suite);
        }
        return false;
    }

    // parameters: '(' [typedargslist] ')'
    PARSER(parameters)
    {
        if (PARSE(LPAR))
        {
            PARSE(typedargslist);
            return PARSE(RPAR);
        }
        return false;
    }

    // typedargslist: (
    //      tfpdef ['=' test] (',' tfpdef ['=' test])*
    //      [
    //          ','
    //          [
    //              '*'
    //              [tfpdef]
    //              (',' tfpdef ['=' test])*
    //              [',' ['**' tfpdef [',']]]
    //                  | '**' tfpdef [',']
    //          ]
    //      ]
    //  | '*' [tfpdef] (',' tfpdef ['=' test])* [',' ['**' tfpdef [',']]]
    //  | '**' tfpdef [','])
    PARSER(typedargslist)
    {
        if (PARSE(tfpdef))
        {
            // tfpdef ['=' test] (',' tfpdef ['=' test])*
            if (PARSE(EQ) && !PARSE(test)) return false;
            while (PARSE(COMMA))
            {
                if (!PARSE(tfpdef)) return false;
                if (PARSE(EQ) && !PARSE(test)) return false;
            }
            // [
            //     ','
            //     [
            //         '*'
            //         [tfpdef]
            //         (',' tfpdef ['=' test])*
            //         [',' ['**' tfpdef [',']]]
            //             | '**' tfpdef [',']
            //     ]
            // ]
            if (PARSE(COMMA))
            {
                if (PARSE(MUL))
                {
                    PARSE(tfpdef);
                    while (true)
                    {
                        if (!PARSE(COMMA))
                            break;
                        if (PARSE(tfpdef))
                        {
                            // We parse (',' tfpdef ['=' test])*
                            if (PARSE(EQ) && !PARSE(test))
                                return false;
                        }
                        else
                        {
                            if (PARSE(POW))
                            {
                                if (!PARSE(tfpdef)) return false;
                                PARSE(COMMA);
                            }
                            break;
                        }
                    }
                }
                else if (PARSE(POW))
                {
                    if (!PARSE(tfpdef)) return false;
                    PARSE(COMMA);
                }
            }
            return true;
        }
        else if (PARSE(MUL))
        {
            //  | '*' [tfpdef] (',' tfpdef ['=' test])* [',' ['**' tfpdef [',']]]
            PARSE(tfpdef);
            while (true)
            {
                if (!PARSE(COMMA))
                    break;
                if (PARSE(tfpdef))
                {
                    // We parse (',' tfpdef ['=' test])*
                    if (PARSE(EQ) && !PARSE(test))
                        return false;
                }
                else
                {
                    // [',' ['**' tfpdef [',']]]
                    if (PARSE(POW))
                    {
                        if (!PARSE(tfpdef)) return false;
                        PARSE(COMMA);
                    }
                    break;
                }
            }
            return true;
        }
        else if (PARSE(POW))
        {
            //  | '**' tfpdef [','])
            if (!PARSE(tfpdef)) return false;
            PARSE(COMMA);
            return true;
        }
        return false;
    }

    // tfpdef: NAME [':' test]
    PARSER(tfpdef)
    {
        if (PARSE(IDENTIFIER))
        {
            if (PARSE(COLON) && !PARSE(test)) return false;
            return true;
        }
        return false;
    }

    // varargslist: (
    // 1      vfpdef ['=' test]
    // 2      (',' vfpdef ['=' test])*
    // 3      [
    // 4          ','
    // 5          [
    // 6              '*'
    // 7              [vfpdef]
    // 8              (',' vfpdef ['=' test])*
    // 9              [',' ['**' vfpdef [',']]]
    // 10             | '**' vfpdef [',']
    // 11         ]
    // 12     ]
    // 13     | '*' [vfpdef] (',' vfpdef ['=' test])* [',' ['**' vfpdef [',']]]
    // 14     | '**' vfpdef [',']
    // )
    PARSER(varargslist)
    {
        if (PARSE(vfpdef)) // 1
        {
            if (PARSE(EQ) && !PARSE(test)) // 1
                return false;
            while (true)
            {
                if (!PARSE(COMMA)) break; // 2
                if (PARSE(vfpdef)) // 2
                {
                    if (PARSE(EQ) && !PARSE(test)) return false;
                }
                else // 5
                {
                    if (PARSE(MUL)) // 6
                    {
                        PARSE(vfpdef); // 7
                        while (true)
                        {
                            if (!PARSE(COMMA)) break; // 8
                            if (PARSE(vfpdef)) // 8
                            {
                                if (PARSE(EQ) && !PARSE(test)) return false;
                            }
                            else // 9
                            {
                                if (PARSE(MUL))
                                {
                                    if (!PARSE(vfpdef)) return false;
                                    PARSE(COMMA);
                                }
                                break;
                            }
                        }
                    }
                    else if (PARSE(POW)) // 10
                    {
                        if (!PARSE(vfpdef)) return false;
                        PARSE(COMMA);
                    }
                    break;
                }
            }
            return true;
        }
        else if (PARSE(MUL))
        {
            // 13     | '*' [vfpdef] (',' vfpdef ['=' test])* [',' ['**' vfpdef [',']]]
            PARSE(vfpdef);
            while (true)
            {
                if (!PARSE(COMMA)) break;
                if (PARSE(vfpdef))
                {
                    if (PARSE(EQ) && !PARSE(test)) return false;
                }
                else
                {
                    if (PARSE(POW))
                    {
                        if (!PARSE(vfpdef)) return false;
                        PARSE(COMMA);
                    }
                    break;
                }
            }
            return true;
        }
        else if (PARSE(POW))
        {
            // 14     | '**' vfpdef [',']
            if (!PARSE(vfpdef)) return false;
            PARSE(COMMA);
            return true;
        }
        return false;
    }

    // vfpdef: NAME
    PARSER(vfpdef) { return PARSE(IDENTIFIER); }

    // stmt: simple_stmt | compound_stmt
    PARSER(stmt) { return PARSE(simple_stmt) || PARSE(compound_stmt); }

    // simple_stmt: small_stmt (';' small_stmt)* [';'] NEWLINE
    PARSER(simple_stmt)
    {
        if (!PARSE(small_stmt)) return false;
        while (PARSE(SEMICOLON))
        {
            if (!PARSE(small_stmt)) break;
        }
        return PARSE(NEWLINE);
    }

    // small_stmt: (expr_stmt | del_stmt | pass_stmt | flow_stmt |
    //             import_stmt | global_stmt | nonlocal_stmt | assert_stmt)
    PARSER(small_stmt)
    {
        return PARSE(expr_stmt) || PARSE(del_stmt) || PARSE(pass_stmt) ||
               PARSE(flow_stmt) || PARSE(import_stmt) || PARSE(global_stmt) ||
               PARSE(nonlocal_stmt) || PARSE(assert_stmt);
    }

    // expr_stmt: testlist_star_expr (augassign (yield_expr|testlist) |
    //                     ('=' (yield_expr|testlist_star_expr))*)
    PARSER(expr_stmt)
    {
        if (!PARSE(testlist_star_expr))
            return false;
        if (PARSE(augassign))
            return PARSE(yield_expr) || PARSE(testlist);
        while (PARSE(EQ))
        {
            if (!(PARSE(yield_expr) || PARSE(testlist)))
                return false;
        }
        return true;
    }

    // testlist_star_expr: (test|star_expr) (',' (test|star_expr))* [',']
    PARSER(testlist_star_expr)
    {
        if (!(PARSE(test) || PARSE(star_expr))) return false;
        while (PARSE(COMMA))
        {
            if (!(PARSE(test) || PARSE(star_expr))) break;
        }
        return true;
    }

    // augassign: ('+=' | '-=' | '*=' | '@=' | '/=' | '%=' | '&=' | '|=' |
    // '^=' |
    //            '<<=' | '>>=' | '**=' | '//=')
    TERMINAL_PARSER(augassign, delimiter)
    {
        return PARSE_STR("+=") || PARSE_STR("-=") || PARSE_STR("*=") ||
               PARSE_STR("@=") || PARSE_STR("/=") || PARSE_STR("%=") ||
               PARSE_STR("&=") || PARSE_STR("|=") || PARSE_STR("^=") ||
               PARSE_STR("<=") || PARSE_STR(">>=") || PARSE_STR("**=") ||
               PARSE_STR("//=");
    }

    //# For normal assignments, additional restrictions enforced by the
    //interpreter
    // del_stmt: 'del' exprlist
    PARSER(del_stmt) { return PARSE(DEL) && PARSE(exprlist); }

    // pass_stmt: 'pass'
    PARSER(pass_stmt) { return PARSE(PASS); }

    // flow_stmt: break_stmt | continue_stmt | return_stmt | raise_stmt |
    // yield_stmt
    PARSER(flow_stmt)
    {
        return PARSE(break_stmt) || PARSE(continue_stmt) ||
               PARSE(return_stmt) || PARSE(raise_stmt) || PARSE(yield_stmt);
    }

    // break_stmt: 'break'
    PARSER(break_stmt) { return PARSE(BREAK); }

    // continue_stmt: 'continue'
    PARSER(continue_stmt) { return PARSE(CONTINUE); }

    // return_stmt: 'return' [testlist]
    PARSER(return_stmt)
    {
        if (!PARSE(RETURN)) return false;
        PARSE(testlist);
        return true;
    }

    // yield_stmt: yield_expr
    PARSER(yield_stmt) { return PARSE(yield_expr); }

    // raise_stmt: 'raise' [test ['from' test]]
    PARSER(raise_stmt)
    {
        if (!PARSE(RAISE)) return false;
        if (PARSE(test))
        {
            if (PARSE(FROM) && !PARSE(test)) return false;
        }
        return true;
    }

    // import_stmt: import_name | import_from
    PARSER(import_stmt) { return PARSE(import_name) || PARSE(import_from); }

    // import_name: 'import' dotted_as_names
    PARSER(import_name) { return PARSE(IMPORT) && PARSE(dotted_as_names); }

    //# note below: the ('.' | '...') is necessary because '...' is
    //tokenized as
    // ELLIPSIS
    // import_from: 'from' (('.' | '...')* dotted_name | ('.' | '...')+)
    //              'import' ('*' | '(' import_as_names ')' | import_as_names)
    PARSER(import_from)
    {
        if (!PARSE(FROM)) return false;
        bool as_dot = false;
        while (PARSE(DOT) || PARSE(ELLIPSIS)) { as_dot = true; }
        if (!PARSE(dotted_name) && !as_dot) return false;
        return PARSE(IMPORT) &&
               (PARSE(MUL) ||
                (PARSE(LPAR) && PARSE(import_as_names) && PARSE(RPAR)) ||
                PARSE(import_as_names));
    }

    // import_as_name: NAME ['as' NAME]
    PARSER(import_as_name)
    {
        if (!PARSE(IDENTIFIER)) return false;
        if (PARSE(AS) && !PARSE(IDENTIFIER)) return false;
        return true;
    }

    // dotted_as_name: dotted_name ['as' NAME]
    PARSER(dotted_as_name)
    {
        if (!PARSE(dotted_name)) return false;
        if (PARSE(AS) && !PARSE(IDENTIFIER)) return false;
        return true;
    }

    // import_as_names: import_as_name (',' import_as_name)* [',']
    PARSER(import_as_names)
    {
        if (!PARSE(import_as_name)) return false;
        while (PARSE(COMMA))
            if (!PARSE(import_as_name)) break;
        return true;
    }

    // dotted_as_names: dotted_as_name (',' dotted_as_name)*
    PARSER(dotted_as_names)
    {
        if (!PARSE(dotted_as_name)) return false;
        while (PARSE(COMMA))
            if (!PARSE(dotted_as_name)) return false;
        return true;
    }

    // dotted_name: NAME ('.' NAME)*
    PARSER(dotted_name)
    {
        if (!PARSE(IDENTIFIER)) return false;
        while (PARSE(DOT))
            if (!PARSE(IDENTIFIER)) return false;
        return true;
    }

    // global_stmt: 'global' NAME (',' NAME)*
    PARSER(global_stmt)
    {
        if (!(PARSE(GLOBAL) && PARSE(IDENTIFIER))) return false;
        while (PARSE(COMMA))
            if (!PARSE(IDENTIFIER)) return false;
        return true;
    }

    // nonlocal_stmt: 'nonlocal' NAME (',' NAME)*
    PARSER(nonlocal_stmt)
    {
        if (!(PARSE(LOCAL) && PARSE(IDENTIFIER))) return false;
        while (PARSE(COMMA))
            if (!PARSE(IDENTIFIER)) return false;
        return true;
    }

    // assert_stmt: 'assert' test [',' test]
    PARSER(assert_stmt)
    {
        if (!(PARSE(ASSERT) && PARSE(test))) return false;
        while (PARSE(COMMA)) if (!PARSE(test)) return false;
        return true;
    }

    //
    // compound_stmt: if_stmt | while_stmt | for_stmt | try_stmt | with_stmt
    // |
    // funcdef | classdef | decorated | async_stmt
    PARSER(compound_stmt)
    {
        return PARSE(if_stmt) || PARSE(while_stmt) || PARSE(for_stmt) ||
               PARSE(try_stmt) || PARSE(with_stmt) || PARSE(funcdef) ||
               PARSE(classdef) || PARSE(decorated) || PARSE(async_stmt);
    }

    // async_stmt: ASYNC (funcdef | with_stmt | for_stmt)
    PARSER(async_stmt)
    {
        return PARSE(ASYNC) &&
               (PARSE(funcdef) || PARSE(with_stmt) || PARSE(for_stmt));
    }

    // if_stmt: 'if' test ':' suite ('elif' test ':' suite)* ['else' ':'
    // suite]
    PARSER(if_stmt)
    {
        if (!(PARSE(IF) && PARSE(test) && PARSE(COLON) && PARSE(suite)))
            return false;
        while (PARSE(ELIF))
        {
            if (!(PARSE(test) && PARSE(COLON) && PARSE(suite)))
                return false;
        }
        if (PARSE(ELSE))
            return PARSE(COLON) && PARSE(suite);
        return true;
    }

    // while_stmt: 'while' test ':' suite ['else' ':' suite]
    PARSER(while_stmt)
    {
        if (!(PARSE(WHILE) && PARSE(test) && PARSE(COLON) && PARSE(suite)))
            return false;
        if (PARSE(ELSE))
            return PARSE(COLON) && PARSE(suite);
        return true;
    }


    // for_stmt: 'for' exprlist 'in' testlist ':' suite ['else' ':' suite]
    PARSER(for_stmt)
    {
        if (!PARSE(FOR) && PARSE(exprlist) && PARSE(IN) && PARSE(testlist) &&
            PARSE(COLON) && PARSE(suite))
            return false;
        if (PARSE(ELSE))
            return PARSE(COLON) && PARSE(suite);
        return true;
    }

    // try_stmt: 'try' ':' suite
    //           ((except_clause ':' suite)+
    //            ['else' ':' suite]
    //            ['finally' ':' suite] |
    //           'finally' ':' suite)
    PARSER(try_stmt)
    {
        if (!PARSE(TRY) && PARSE(COLON) && PARSE(suite)) return false;
        if (PARSE(except_clause))
        {
            do {
                if (!(PARSE(COLON) && PARSE(suite))) return false;
            } while (PARSE(except_clause));

            if (PARSE(ELSE))
                if (!PARSE(COLON) && PARSE(suite)) return false;
            if (PARSE(FINALLY))
                if (!PARSE(COLON) && PARSE(suite)) return false;
        }
        else
            return PARSE(FINALLY) && PARSE(COLON) && PARSE(suite);
        return true;
    }

    // with_stmt: 'with' with_item (',' with_item)*  ':' suite
    PARSER(with_stmt)
    {
        if (!(PARSE(WITH) && PARSE(with_item))) return false;
        while (PARSE(COMMA))
            if (!PARSE(with_item)) return false;
        return PARSE(COLON) && PARSE(suite);
    }

    // with_item: test ['as' expr]
    PARSER(with_item)
    {
        if (!PARSE(test)) return false;
        if (PARSE(AS)) return PARSE(expr);
        return true;
    }

    //# NB compile.c makes sure that the default except clause is last
    // except_clause: 'except' [test ['as' NAME]]
    PARSER(except_clause)
    {
        if (!PARSE(EXCEPT)) return false;
        if (PARSE(test))
        {
            if (PARSE(AS)) return PARSE(IDENTIFIER);
        }
        return true;
    }

    // suite: simple_stmt | NEWLINE INDENT stmt+ DEDENT
    PARSER(suite)
    {
        if (PARSE(simple_stmt)) return true;
        if (!(PARSE(NEWLINE) && PARSE(INDENT) && PARSE(stmt))) return false;
        while (PARSE(stmt)) {}
        return PARSE(DEDENT);
    }


    // test: or_test ['if' or_test 'else' test] | lambdef
    PARSER(test)
    {
        if (PARSE(or_test))
        {
            if (PARSE(IF))
            {
                if (!(PARSE(or_test) && PARSE(ELSE) && PARSE(test))) return false;
            }
            return true;
        }
        return PARSE(lambdef);
    }

    // test_nocond: or_test | lambdef_nocond
    PARSER(test_nocond)
    {
        return PARSE(or_test) || PARSE(lambdef_nocond);
    }

    // lambdef: 'lambda' [varargslist] ':' test
    PARSER(lambdef)
    {
        if (!PARSE(LAMBDA)) return false;
        PARSE(varargslist);
        return PARSE(COLON) && PARSE(test);
    }

    // lambdef_nocond: 'lambda' [varargslist] ':' test_nocond
    PARSER(lambdef_nocond)
    {
        if (!PARSE(LAMBDA)) return false;
        PARSE(varargslist);
        return PARSE(COLON) && PARSE(test_nocond);
    }

    // or_test: and_test ('or' and_test)*
    PARSER(or_test)
    {
        if (!PARSE(and_test)) return false;
        while (PARSE(OR)) if (!PARSE(and_test)) return false;
        return true;
    }

    // and_test: not_test ('and' not_test)*
    PARSER(and_test)
    {
        if (!PARSE(not_test)) return false;
        while (PARSE(AND)) if (!PARSE(not_test)) return false;
        return true;
    }

    // not_test: 'not' not_test | comparison
    PARSER(not_test)
    {
        if (PARSE(NOT)) return PARSE(not_test);
        return PARSE(comparison);
    }

    // comparison: expr (comp_op expr)*
    PARSER(comparison)
    {
        if (!PARSE(expr)) return false;
        while (PARSE(comp_op))
            if (!PARSE(expr)) return false;
        return true;
    }

    //# <> isn't actually a valid comparison operator in Python. It's here
    //for the
    //# sake of a __future__ import described in PEP 401 (which really works
    //:-)
    // comp_op: '<'|'>'|'=='|'>='|'<='|'<>'|'!='|'in'|'not' 'in'|'is'|'is'
    // 'not'
    TERMINAL_PARSER(comp_op, operator_)
    {
        return PARSE_STR("<") || PARSE_STR(">") || PARSE_STR("==") ||
               PARSE_STR(">=") || PARSE_STR("<=") || PARSE_STR("<>") ||
               PARSE_STR("!=") || PARSE_STR("in") ||
               (PARSE_STR("not") && PARSE_STR("in")) || PARSE_STR("is") ||
               (PARSE_STR("is") && PARSE_STR("not"));
    }

    // star_expr: '*' expr
    PARSER(star_expr) { return PARSE(MUL) && PARSE(expr); }

    // expr: xor_expr ('|' xor_expr)*
    PARSER(expr)
    {
        if (!PARSE(xor_expr)) return false;
        while (PARSE(OP_OR)) if (!PARSE(xor_expr)) return false;
        return true;
    }

    // xor_expr: and_expr ('^' and_expr)*
    PARSER(xor_expr)
    {
        if (!PARSE(and_expr)) return false;
        while (PARSE(OP_XOR)) if (!PARSE(and_expr)) return false;
        return true;
    }

    // and_expr: shift_expr ('&' shift_expr)*
    PARSER(and_expr)
    {
        if (!PARSE(shift_expr)) return false;
        while (PARSE(OP_AND)) if (!PARSE(shift_expr)) return false;
        return true;
    }

    // shift_expr: arith_expr (('<<'|'>>') arith_expr)*
    PARSER(shift_expr)
    {
        if (!PARSE(arith_expr)) return false;
        while (PARSE(OP_LSHIFT) || PARSE(OP_RSHIFT))
            if (!PARSE(arith_expr)) return false;
        return true;
    }

    // arith_expr: term (('+'|'-') term)*
    PARSER(arith_expr)
    {
        if (!PARSE(term)) return false;
        while (PARSE(OP_ADD) || PARSE(OP_SUB))
            if (!PARSE(term)) return false;
        return true;
    }

    // term: factor (('*'|'@'|'/'|'%'|'//') factor)*
    PARSER(term) {}
    // factor: ('+'|'-'|'~') factor | power
    PARSER(factor) {}
    // power: atom_expr ['**' factor]
    PARSER(power) {}
    // atom_expr: [AWAIT] atom trailer*
    PARSER(atom_expr) {}
    // atom: ('(' [yield_expr|testlist_comp] ')' |
    //       '[' [testlist_comp] ']' |
    //       '{' [dictorsetmaker] '}' |
    //       NAME | NUMBER | STRING+ | '...' | 'None' | 'True' | 'False')
    PARSER(atom) {}
    // testlist_comp: (test|star_expr) ( comp_for | (',' (test|star_expr))*
    // [','] )
    PARSER(testlist_comp) {}
    // trailer: '(' [arglist] ')' | '[' subscriptlist ']' | '.' NAME
    PARSER(trailer) {}
    // subscriptlist: subscript (',' subscript)* [',']
    PARSER(subscriptlist) {}
    // subscript: test | [test] ':' [test] [sliceop]
    PARSER(subscript) {}
    // sliceop: ':' [test]
    PARSER(sliceop) {}
    // exprlist: (expr|star_expr) (',' (expr|star_expr))* [',']
    PARSER(exprlist) {}
    // testlist: test (',' test)* [',']
    PARSER(testlist) {}
    // dictorsetmaker: ( ((test ':' test | '**' expr)
    //                   (comp_for | (',' (test ':' test | '**' expr))*
    //                   [','])) |
    //                  ((test | star_expr)
    //                   (comp_for | (',' (test | star_expr))* [','])) )
    //
    PARSER(dictorsetmaker) {}
    // classdef: 'class' NAME ['(' [arglist] ')'] ':' suite
    PARSER(classdef) {}
    //
    // arglist: argument (',' argument)*  [',']
    PARSER(arglist) {}
    //
    //# The reason that keywords are test nodes instead of NAME is that
    //using NAME
    //# results in an ambiguity. ast.c makes sure it's a NAME.
    //# "test '=' test" is really "keyword '=' test", but we have no such
    //token.
    //# These need to be in a single rule to avoid grammar that is ambiguous
    //# to our LL(1) parser. Even though 'test' includes '*expr' in
    //star_expr,
    //# we explicitly match '*' here, too, to give it proper precedence.
    //# Illegal combinations and orderings are blocked in ast.c:
    //# multiple (test comp_for) arguements are blocked; keyword unpackings
    //# that precede iterable unpackings are blocked; etc.
    // argument: ( test [comp_for] |
    //            test '=' test |
    //            '**' test |
    //            '*' test )
    PARSER(argument) {}
    //
    // comp_iter: comp_for | comp_if
    PARSER(comp_iter) {}
    // comp_for: 'for' exprlist 'in' or_test [comp_iter]
    PARSER(comp_for) {}
    // comp_if: 'if' test_nocond [comp_iter]
    PARSER(comp_if) {}
    //
    //# not used in grammar, but may appear in "node" passed from Parser to
    //Compiler
    // encoding_decl: NAME
    PARSER(encoding_decl) {}
    //
    // yield_expr: 'yield' [yield_arg]
    PARSER(yield_expr) {}
    // yield_arg: 'from' test | testlist
    PARSER(yield_arg) {}

    // single_input: NEWLINE | simple_stmt | compound_stmt NEWLINE
    PARSER(single_input) { return parse_simple_stmt(tok, range); }

    // file_input: (NEWLINE | stmt)* ENDMARKER
    PARSER(file_input) {}

    // eval_input: testlist NEWLINE* ENDMARKER
    PARSER(eval_input) {}

    Lexer::Lexer(Source& source) : _source(source), _current(_source.begin()) {}
    bool Lexer::next_token(Token& tok, SourceRange& range)
    {
    restart:
        range.begin = _current;
        if (_current == _source.end()) range.end = _current;
        switch (*_current++)
        {
        case '\0':
            tok = Token::eof;
            return true;

        case ' ':
        case '\t':
        case '\f':
        case '\v':
            _current++;
            goto restart;
        case '\n':
            tok = Token::eol;
            return true;
        }

        return false;
    }

}}
