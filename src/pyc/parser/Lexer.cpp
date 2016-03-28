#include "Lexer.hpp"
#include "Source.hpp"
#include "SourceRange.hpp"
#include "Token.hpp"

#include <vector>
#include <cassert>
#include <iostream>

namespace pyc { namespace parser {

    typedef std::vector<std::pair<Token, SourceRange>> Stack;

    struct Context
    {
        unsigned indent = 0;
        std::vector<char const*> elements;
        void log(Stack& stack, char const* str)
        {
            return;
//std::cout << std::string(elements.size(), ' ') << elements.back()
//                  << ' ' << str << std::endl;
            std::cout << str << ' ';
            for (auto e: elements)
                std::cout << e << ' ';
            std::cout << std::endl << " --> ";
            for (auto& pair: stack)
            {
                if (pair.first >= Token::_keyword_begin &&
                    pair.first < Token::_keyword_end)
                {
                    std::cout << std::string(pair.second.begin, pair.second.end)
                              << ' ';
                }
                else
                    std::cout << pair.first << ' ';
            }
            std::cout << std::endl;
        }

#define PARSER(name)                                     \
    bool parse_##name(Stack& stack, SourceLocation& loc) \
    {                                                    \
        SourceLocation old = loc;                        \
        auto stack_size = stack.size();                  \
        indent += 1;                                     \
        log(stack, ">>");                                     \
        elements.push_back(#name);                       \
        if (!parse_##name##_body(stack, loc)) {          \
            loc = old;                                   \
            assert(stack_size <= stack.size());          \
            stack.resize(stack_size);                    \
            indent -= 1;                                 \
            log(stack, "EE");                                     \
            elements.pop_back();                         \
            return false;                                \
        }                                                \
            log(stack, "YY");                                     \
        elements.pop_back();                             \
        indent -= 1;                                     \
        return true;                                     \
    }                                                    \
    bool parse_##name##_body(Stack& stack, SourceLocation& loc)

#define PARSER_DECL(name) bool parse_##name(Stack& stack, SourceLocation& loc)
#define PARSE(name) parse_##name(stack, loc)

#define TERMINAL_PARSER(name, token)                                      \
    PARSER(name)                                                          \
    {                                                                     \
        SourceLocation old = loc;                                         \
        log(stack, ">>>");                                     \
        stack.emplace_back(Token::token, SourceRange{old, loc});          \
        if (parse_terminal_##name##_body(stack, loc)) {                   \
            stack.back().second.end = loc;                                \
            if (Token::token != Token::newline)                           \
                while (*loc == ' ') ++loc;                                \
            log(stack, "YYY");                                     \
            return true;                                                  \
        }                                                                 \
        else                                                              \
        {                                                                 \
            if (*loc == '\0') {                                           \
                /*std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ EOF\n"; */\
            }                                                             \
        }                                                                 \
        stack.pop_back();                                                 \
        log(stack, "EEE");                                    \
        return false;                                                     \
    }                                                                     \
    bool parse_terminal_##name##_body(Stack& stack, SourceLocation& loc)

        bool parse_string(SourceLocation& loc, char const* str)
        {
            SourceLocation old = loc;
            assert(str != nullptr);
            while (*str != '\0' && *str == *loc) {
                str++;
                loc++;
            }
            if (*str != '\0') {
                if (*loc == '\0')
                {
                    //std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ EOF\n";
                }
                loc = old;
                return false;
            }
            return true;
        }

#define PARSE_STR(str) parse_string(loc, str)

#define TERMINAL_PARSER_STR(name, str, token) \
    TERMINAL_PARSER(name, token) { return PARSE_STR(str); }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

        TERMINAL_PARSER(NEWLINE, newline)
        {
            if (PARSE_STR("\r") || PARSE_STR("\n")) {
                // while (PARSE_STR("\r") || PARSE_STR("\n")) {}
                return true;
            }
            return false;
        }

        TERMINAL_PARSER(IDENTIFIER, identifier)
        {
            auto start = loc;
            while ((*loc >= 'a' && *loc <= 'z') || (*loc >= 'A' && *loc <= 'Z'))
                ++loc;
            return start != loc;
        }

        std::vector<unsigned> block_indents = {0};
        TERMINAL_PARSER(INDENT, indent)
        {
            assert(block_indents.size() > 0);
            unsigned i = 0;
            while (PARSE_STR(" ") || PARSE_STR("\t")) i += 1;
            if (i < block_indents.back()) {
                std::cout << "WRONG INDENT: expected >= "
                          << block_indents.back() << " got " << i << std::endl;
                return false;
            }
            block_indents.push_back(i);
            return true;
        }

        TERMINAL_PARSER(DEDENT, dedent)
        {
            assert(block_indents.size() > 0);
            unsigned i = 0;
            while (PARSE_STR(" ") || PARSE_STR("\t")) i += 1;
            auto it = block_indents.rbegin(), end = block_indents.rend();
            for (; it != end; ++it)
                if (*it <= i) break;
            if (*it != i) {
                std::cout << "WRONG DEDENT: expected " << *it << " got " << i
                          << std::endl;
                return false;
            }
            block_indents.pop_back();
            return true;
        }

        TERMINAL_PARSER(NUMBER, number)
        {
            auto start = loc;
            while (*loc >= '0' && *loc <= '9') ++loc;
            return start != loc;
        }

        PARSER(string_literal)
        {

            bool bytes = false;
            bool raw = false;
            bool formatted = false;
            {
                bool unicode = false;
parse_flag:     switch (*loc)
                {
                case 'b':
                case 'B':
                    bytes = true;
                    ++loc; goto parse_flag;
                case 'r':
                case 'R':
                    raw = true;
                    ++loc; goto parse_flag;
                    break;
                case 'u':
                case 'U':
                    unicode = true;
                    ++loc; goto parse_flag;
                    break;
                case 'f':
                case 'F':
                    formatted = true;
                    ++loc; goto parse_flag;
                case '"':
                case '\'':
                    break;
                default:
                    return false;
                }
                if (bytes && formatted)
                    throw "wat?";
                if (bytes && unicode)
                    throw "wat?";
            }
            char delimiter = *loc;
            int delim_size = 1;
            if (*(loc + 1) == delimiter && *(loc + 2) == delimiter)
            {
                delim_size = 3;
            }
            loc += delim_size;

            bool ignore_next = false;
            auto start = loc;
            while (*loc != '\0')
            {
                if (ignore_next)
                {
                    ignore_next = false;
                    loc += 1;
                    continue;
                }
                if (*loc == '\\')
                {
                    ignore_next = true;
                    loc += 1;
                    continue;
                }
                auto end = loc;
                int i = 0;
                while (*loc == delimiter && i < delim_size)
                {
                    loc += 1;
                    i += 1;
                }
                if (i == 0)
                    loc += 1;
                else if (i == delim_size)
                {
                    Token token;
                    if (bytes)
                        token = raw ? Token::raw_bytes : Token::bytes;
                    else if (formatted)
                        token = raw ? Token::raw_formatted_string : Token::formatted_string;
                    else
                        token = raw ? Token::raw_unicode : Token::unicode;
                    stack.emplace_back(token, SourceRange{start, end});
                    return true;
                }
                else if (delim_size == 1 && (*loc == '\r' || *loc == '\n'))
                    throw "wat?";
            }
            return false;
        }

        TERMINAL_PARSER(ENDMARKER, eof) { return *loc == '\0'; }

        TERMINAL_PARSER_STR(AT, "@", at);
        TERMINAL_PARSER_STR(COLON, ":", colon);
        TERMINAL_PARSER_STR(COMMA, ",", comma);
        TERMINAL_PARSER_STR(EQ, "=", equal);
        TERMINAL_PARSER_STR(LPAR, "(", lpar);
        TERMINAL_PARSER_STR(MUL, "*", star);
        TERMINAL_PARSER_STR(POW, "**", double_star);
        TERMINAL_PARSER_STR(RARROW, "->", right_arrow);
        TERMINAL_PARSER_STR(RPAR, ")", rpar);
        TERMINAL_PARSER_STR(SEMICOLON, ";", semicolon);
        TERMINAL_PARSER_STR(LSQUAREBRACKET, "[", lsqb);
        TERMINAL_PARSER_STR(RSQUAREBRACKET, "]", rsqb);
        TERMINAL_PARSER_STR(LBRACKET, "{", left_brace);
        TERMINAL_PARSER_STR(RBRACKET, "}", right_brace);

        TERMINAL_PARSER_STR(AS, "as", as);
        TERMINAL_PARSER_STR(ASYNC, "async", async);
        TERMINAL_PARSER_STR(BREAK, "break", break_);
        TERMINAL_PARSER_STR(CONTINUE, "continue", continue_);
        TERMINAL_PARSER_STR(DEF, "def", def);
        TERMINAL_PARSER_STR(DEL, "del", del);
        TERMINAL_PARSER_STR(DOT, ".", dot);
        TERMINAL_PARSER_STR(ELLIPSIS, "...", ellipsis);
        TERMINAL_PARSER_STR(FROM, "from", from);
        TERMINAL_PARSER_STR(IMPORT, "import", import);
        TERMINAL_PARSER_STR(PASS, "pass", pass);
        TERMINAL_PARSER_STR(RAISE, "raise", raise);
        TERMINAL_PARSER_STR(RETURN, "return", return_);
        TERMINAL_PARSER_STR(GLOBAL, "global", global);
        TERMINAL_PARSER_STR(NONLOCAL, "local", nonlocal);
        TERMINAL_PARSER_STR(ASSERT, "assert", assert_);
        TERMINAL_PARSER_STR(IF, "if", if_);
        TERMINAL_PARSER_STR(ELIF, "elif", elif);
        TERMINAL_PARSER_STR(ELSE, "else", else_);
        TERMINAL_PARSER_STR(WHILE, "while", while_);
        TERMINAL_PARSER_STR(FOR, "for", for_);
        TERMINAL_PARSER_STR(TRY, "try", try_);
        TERMINAL_PARSER_STR(FINALLY, "finally", finally);
        TERMINAL_PARSER_STR(WITH, "with", with);
        TERMINAL_PARSER_STR(EXCEPT, "except", except);
        TERMINAL_PARSER_STR(LAMBDA, "lambda", lambda);
        TERMINAL_PARSER_STR(OR, "or", or_);
        TERMINAL_PARSER_STR(AND, "and", and_);
        TERMINAL_PARSER_STR(NOT, "not", not_);
        TERMINAL_PARSER_STR(AWAIT, "await", await);
        TERMINAL_PARSER_STR(CLASS, "class", class_);
        TERMINAL_PARSER_STR(YIELD, "yield", yield);

        TERMINAL_PARSER_STR(OP_OR, "|", pipe);
        TERMINAL_PARSER_STR(OP_XOR, "^", circumflex);
        TERMINAL_PARSER_STR(OP_AND, "&", ampersand);
        TERMINAL_PARSER_STR(OP_RSHIFT, ">>", right_shift);
        TERMINAL_PARSER_STR(OP_LSHIFT, "<<", left_shift);
        TERMINAL_PARSER_STR(OP_ADD, "+", plus);
        TERMINAL_PARSER_STR(OP_SUB, "-", minus);
        TERMINAL_PARSER_STR(OP_MUL, "*", star);
        TERMINAL_PARSER_STR(OP_MATMUL, "@", at);
        TERMINAL_PARSER_STR(OP_DIV, "/", slash);
        TERMINAL_PARSER_STR(OP_MOD, "%", percent);
        TERMINAL_PARSER_STR(OP_TRUEDIV, "//", double_slash);
        TERMINAL_PARSER_STR(OP_NOT, "~", tilde);
        TERMINAL_PARSER_STR(OP_POW, "**", double_star);

        TERMINAL_PARSER_STR(NONE, "None", identifier);
        TERMINAL_PARSER_STR(TRUE, "True", identifier);
        TERMINAL_PARSER_STR(FALSE, "False", identifier);

        // decorator: '@' dotted_name [ '(' [arglist] ')' ] NEWLINE
        PARSER(decorator)
        {
            if (!(PARSE(AT) && PARSE(dotted_name))) return false;
            if (PARSE(LPAR)) {
                PARSE(arglist);
                if (!PARSE(RPAR)) return false;
            }
            return PARSE(NEWLINE);
        }

        // decorators: decorator+
        PARSER(decorators)
        {
            if (PARSE(decorator)) {
                while (PARSE(decorator)) {
                }
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
        PARSER(async_funcdef) { return PARSE(ASYNC) && PARSE(funcdef); }

        // funcdef: 'def' NAME parameters ['->' test] ':' suite
        PARSER(funcdef)
        {
            if (PARSE(DEF) && PARSE(IDENTIFIER) && PARSE(parameters)) {
                if (PARSE(RARROW) && !PARSE(test)) return false;
                return PARSE(COLON) && PARSE(suite);
            }
            return false;
        }

        // parameters: '(' [typedargslist] ')'
        PARSER(parameters)
        {
            if (PARSE(LPAR)) {
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
            if (PARSE(tfpdef)) {
                // tfpdef ['=' test] (',' tfpdef ['=' test])*
                if (PARSE(EQ) && !PARSE(test)) return false;
                while (PARSE(COMMA)) {
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
                if (PARSE(COMMA)) {
                    if (PARSE(MUL)) {
                        PARSE(tfpdef);
                        while (true) {
                            if (!PARSE(COMMA)) break;
                            if (PARSE(tfpdef)) {
                                // We parse (',' tfpdef ['=' test])*
                                if (PARSE(EQ) && !PARSE(test)) return false;
                            }
                            else
                            {
                                if (PARSE(POW)) {
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
                //  | '*' [tfpdef] (',' tfpdef ['=' test])* [',' ['**' tfpdef
                //  [',']]]
                PARSE(tfpdef);
                while (true) {
                    if (!PARSE(COMMA)) break;
                    if (PARSE(tfpdef)) {
                        // We parse (',' tfpdef ['=' test])*
                        if (PARSE(EQ) && !PARSE(test)) return false;
                    }
                    else
                    {
                        // [',' ['**' tfpdef [',']]]
                        if (PARSE(POW)) {
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
            if (PARSE(IDENTIFIER)) {
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
        // 13     | '*' [vfpdef] (',' vfpdef ['=' test])* [',' ['**' vfpdef
        // [',']]]
        // 14     | '**' vfpdef [',']
        // )
        PARSER(varargslist)
        {
            if (PARSE(vfpdef))  // 1
            {
                if (PARSE(EQ) && !PARSE(test))  // 1
                    return false;
                while (true) {
                    if (!PARSE(COMMA)) break;  // 2
                    if (PARSE(vfpdef))         // 2
                    {
                        if (PARSE(EQ) && !PARSE(test)) return false;
                    }
                    else  // 5
                    {
                        if (PARSE(MUL))  // 6
                        {
                            PARSE(vfpdef);  // 7
                            while (true) {
                                if (!PARSE(COMMA)) break;  // 8
                                if (PARSE(vfpdef))         // 8
                                {
                                    if (PARSE(EQ) && !PARSE(test)) return false;
                                }
                                else  // 9
                                {
                                    if (PARSE(MUL)) {
                                        if (!PARSE(vfpdef)) return false;
                                        PARSE(COMMA);
                                    }
                                    break;
                                }
                            }
                        }
                        else if (PARSE(POW))  // 10
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
                // 13     | '*' [vfpdef] (',' vfpdef ['=' test])* [',' ['**'
                // vfpdef [',']]]
                PARSE(vfpdef);
                while (true) {
                    if (!PARSE(COMMA)) break;
                    if (PARSE(vfpdef)) {
                        if (PARSE(EQ) && !PARSE(test)) return false;
                    }
                    else
                    {
                        if (PARSE(POW)) {
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
            while (PARSE(SEMICOLON)) {
                if (!PARSE(small_stmt)) break;
            }
            return PARSE(NEWLINE);
        }

        // small_stmt: (expr_stmt | del_stmt | pass_stmt | flow_stmt |
        //             import_stmt | global_stmt | nonlocal_stmt | assert_stmt)
        PARSER(small_stmt)
        {
            return PARSE(del_stmt) || PARSE(pass_stmt) ||
                   PARSE(flow_stmt) || PARSE(import_stmt) ||
                   PARSE(global_stmt) || PARSE(nonlocal_stmt) ||
                   PARSE(assert_stmt) || PARSE(expr_stmt);
        }

        // expr_stmt: testlist_star_expr (augassign (yield_expr|testlist) |
        //                     ('=' (yield_expr|testlist_star_expr))*)
        PARSER(expr_stmt)
        {
            if (!PARSE(testlist_star_expr)) return false;
            if (PARSE(augassign)) return PARSE(yield_expr) || PARSE(testlist);
            while (PARSE(EQ)) {
                if (!(PARSE(yield_expr) || PARSE(testlist))) return false;
            }
            return true;
        }

        // testlist_star_expr: (test|star_expr) (',' (test|star_expr))* [',']
        PARSER(testlist_star_expr)
        {
            if (!(PARSE(test) || PARSE(star_expr))) return false;
            while (PARSE(COMMA)) {
                if (!(PARSE(test) || PARSE(star_expr))) break;
            }
            return true;
        }


        TERMINAL_PARSER_STR(PLUS_EQUAL, "+=", plus_equal);
        TERMINAL_PARSER_STR(MINUS_EQUAL, "-=", minus_equal);
        TERMINAL_PARSER_STR(STAR_EQUAL, "*=", star_equal);
        TERMINAL_PARSER_STR(SLASH_EQUAL, "/=", slash_equal);
        TERMINAL_PARSER_STR(PERCENT_EQUAL, "%=", percent_equal);
        TERMINAL_PARSER_STR(AMPERSAND_EQUAL, "&=", ampersand_equal);
        TERMINAL_PARSER_STR(PIPE_EQUAL, "|=", pipe_equal);
        TERMINAL_PARSER_STR(CIRCUMFLEX_EQUAL, "^=", circumflex_equal);
        TERMINAL_PARSER_STR(LEFT_SHIFT_EQUAL, "<<=", left_shift_equal);
        TERMINAL_PARSER_STR(RIGHT_SHIFT_EQUAL, ">>=", right_shift_equal);
        TERMINAL_PARSER_STR(DOUBLE_STAR_EQUAL, "**=", double_star_equal);
        TERMINAL_PARSER_STR(DOUBLE_SLASH_EQUAL, "//=", double_slash_equal);
        TERMINAL_PARSER_STR(AT_EQUAL, "@=", at_equal);


        // augassign: ('+=' | '-=' | '*=' | '@=' | '/=' | '%=' | '&=' | '|=' |
        // '^=' |
        //            '<<=' | '>>=' | '**=' | '//=')
        PARSER(augassign)
        {
            return PARSE(PLUS_EQUAL) || PARSE(MINUS_EQUAL) ||
                   PARSE(STAR_EQUAL) || PARSE(AT_EQUAL) || PARSE(SLASH_EQUAL) ||
                   PARSE(PERCENT_EQUAL) || PARSE(AMPERSAND_EQUAL) ||
                   PARSE(PIPE_EQUAL) || PARSE(CIRCUMFLEX_EQUAL) ||
                   PARSE(LEFT_SHIFT_EQUAL) || PARSE(RIGHT_SHIFT_EQUAL) ||
                   PARSE(DOUBLE_STAR_EQUAL) || PARSE(DOUBLE_SLASH_EQUAL);
        }

        //# For normal assignments, additional restrictions enforced by the
        // interpreter
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
            if (PARSE(test)) {
                if (PARSE(FROM) && !PARSE(test)) return false;
            }
            return true;
        }

        // import_stmt: import_name | import_from
        PARSER(import_stmt) { return PARSE(import_name) || PARSE(import_from); }

        // import_name: 'import' dotted_as_names
        PARSER(import_name) { return PARSE(IMPORT) && PARSE(dotted_as_names); }

        //# note below: the ('.' | '...') is necessary because '...' is
        // tokenized as
        // ELLIPSIS
        // import_from: 'from' (('.' | '...')* dotted_name | ('.' | '...')+)
        //              'import' ('*' | '(' import_as_names ')' |
        //              import_as_names)
        PARSER(import_from)
        {
            if (!PARSE(FROM)) return false;
            bool as_dot = false;
            while (PARSE(DOT) || PARSE(ELLIPSIS)) {
                as_dot = true;
            }
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
            if (!(PARSE(NONLOCAL) && PARSE(IDENTIFIER))) return false;
            while (PARSE(COMMA))
                if (!PARSE(IDENTIFIER)) return false;
            return true;
        }

        // assert_stmt: 'assert' test [',' test]
        PARSER(assert_stmt)
        {
            if (!(PARSE(ASSERT) && PARSE(test))) return false;
            while (PARSE(COMMA))
                if (!PARSE(test)) return false;
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
            while (PARSE(ELIF)) {
                if (!(PARSE(test) && PARSE(COLON) && PARSE(suite)))
                    return false;
            }
            if (PARSE(ELSE)) return PARSE(COLON) && PARSE(suite);
            return true;
        }

        // while_stmt: 'while' test ':' suite ['else' ':' suite]
        PARSER(while_stmt)
        {
            if (!(PARSE(WHILE) && PARSE(test) && PARSE(COLON) && PARSE(suite)))
                return false;
            if (PARSE(ELSE)) return PARSE(COLON) && PARSE(suite);
            return true;
        }

        // for_stmt: 'for' exprlist 'in' testlist ':' suite ['else' ':' suite]
        PARSER(for_stmt)
        {
            if (!(PARSE(FOR) && PARSE(exprlist) && PARSE(IN) &&
                PARSE(testlist) && PARSE(COLON) && PARSE(suite)))
                return false;
            if (PARSE(ELSE)) return PARSE(COLON) && PARSE(suite);
            return true;
        }

        // try_stmt: 'try' ':' suite
        //           ((except_clause ':' suite)+
        //            ['else' ':' suite]
        //            ['finally' ':' suite] |
        //           'finally' ':' suite)
        PARSER(try_stmt)
        {
            if (!(PARSE(TRY) && PARSE(COLON) && PARSE(suite))) return false;
            if (PARSE(except_clause)) {
                do
                {
                    if (!(PARSE(COLON) && PARSE(suite))) return false;
                } while (PARSE(except_clause));

                if (PARSE(ELSE))
                    if (!(PARSE(COLON) && PARSE(suite))) return false;
                if (PARSE(FINALLY))
                    if (!(PARSE(COLON) && PARSE(suite))) return false;
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
            if (PARSE(test)) {
                if (PARSE(AS)) return PARSE(IDENTIFIER);
            }
            return true;
        }

        // suite: simple_stmt | NEWLINE INDENT stmt+ DEDENT
        PARSER(suite)
        {
            if (PARSE(simple_stmt)) return true;
            if (!(PARSE(NEWLINE) && PARSE(INDENT) && PARSE(stmt))) return false;
            while (PARSE(stmt)) {
            }
            return PARSE(DEDENT);
        }

        // test: or_test ['if' or_test 'else' test] | lambdef
        PARSER(test)
        {
            if (PARSE(or_test)) {
                if (PARSE(IF)) {
                    if (!(PARSE(or_test) && PARSE(ELSE) && PARSE(test)))
                        return false;
                }
                return true;
            }
            return PARSE(lambdef);
        }

        // test_nocond: or_test | lambdef_nocond
        PARSER(test_nocond) { return PARSE(or_test) || PARSE(lambdef_nocond); }

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
            while (PARSE(OR))
                if (!PARSE(and_test)) return false;
            return true;
        }

        // and_test: not_test ('and' not_test)*
        PARSER(and_test)
        {
            if (!PARSE(not_test)) return false;
            while (PARSE(AND))
                if (!PARSE(not_test)) return false;
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

        TERMINAL_PARSER_STR(LESS, "<", less);
        TERMINAL_PARSER_STR(GREATER, ">", greater);
        TERMINAL_PARSER_STR(DOUBLE_EQUAL, "==", double_equal);
        TERMINAL_PARSER_STR(NOT_EQUAL, "!=", not_equal);
        TERMINAL_PARSER_STR(LESS_EQUAL, "<=", less_equal);
        TERMINAL_PARSER_STR(GREATER_EQUAL, ">=", greater_equal);

        TERMINAL_PARSER_STR(IN, "in", in);
        TERMINAL_PARSER(NOTIN, not_in)
        {
            return PARSE_STR("not") && PARSE_STR("in");
        }
        TERMINAL_PARSER_STR(IS, "is", is);
        TERMINAL_PARSER(ISNOT, is_not)
        {
            return PARSE_STR("is") && PARSE_STR("not");
        }

        //# <> isn't actually a valid comparison operator in Python. It's here
        // for the
        //# sake of a __future__ import described in PEP 401 (which really works
        //:-)
        // comp_op: '<'|'>'|'=='|'>='|'<='|'<>'|'!='|'in'|'not' 'in'|'is'|'is'
        // 'not'
        PARSER(comp_op)
        {
            return PARSE(LESS) || PARSE(GREATER) || PARSE(DOUBLE_EQUAL) ||
                   PARSE(GREATER_EQUAL) || PARSE(LESS_EQUAL) ||
                   PARSE(NOT_EQUAL) || PARSE(IN) || PARSE(NOTIN) || PARSE(IS) ||
                   PARSE(ISNOT);
        }

        // star_expr: '*' expr
        PARSER(star_expr) { return PARSE(MUL) && PARSE(expr); }

        // expr: xor_expr ('|' xor_expr)*
        PARSER(expr)
        {
            if (!PARSE(xor_expr)) return false;
            while (PARSE(OP_OR))
                if (!PARSE(xor_expr)) return false;
            return true;
        }

        // xor_expr: and_expr ('^' and_expr)*
        PARSER(xor_expr)
        {
            if (!PARSE(and_expr)) return false;
            while (PARSE(OP_XOR))
                if (!PARSE(and_expr)) return false;
            return true;
        }

        // and_expr: shift_expr ('&' shift_expr)*
        PARSER(and_expr)
        {
            if (!PARSE(shift_expr)) return false;
            while (PARSE(OP_AND))
                if (!PARSE(shift_expr)) return false;
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
        PARSER(term)
        {
            if (!PARSE(factor)) return false;
            while (PARSE(OP_MUL) || PARSE(OP_MATMUL) || PARSE(OP_DIV) ||
                   PARSE(OP_MOD) || PARSE(OP_TRUEDIV))
            {
                if (!PARSE(factor)) return false;
            }
            return true;
        }

        // factor: ('+'|'-'|'~') factor | power
        PARSER(factor)
        {
            if (PARSE(OP_ADD) || PARSE(OP_SUB) || PARSE(OP_NOT))
                return PARSE(factor);
            return PARSE(power);
        }

        // power: atom_expr ['**' factor]
        PARSER(power)
        {
            if (!PARSE(atom_expr)) return false;
            if (PARSE(OP_POW)) return PARSE(factor);
            return true;
        }

        // atom_expr: [AWAIT] atom trailer*
        PARSER(atom_expr)
        {
            PARSE(AWAIT);
            if (!PARSE(atom)) return false;
            while (PARSE(trailer)) {
            }
            return true;
        }

        // atom: ('(' [yield_expr|testlist_comp] ')' |
        //       '[' [testlist_comp] ']' |
        //       '{' [dictorsetmaker] '}' |
        //       NAME | NUMBER | STRING+ | '...' | 'None' | 'True' | 'False')
        PARSER(atom)
        {
            if (PARSE(LPAR)) {
                if (!PARSE(yield_expr)) PARSE(testlist_comp);
                return PARSE(RPAR);
            }
            else if (PARSE(LSQUAREBRACKET))
            {
                PARSE(testlist_comp);
                return PARSE(RSQUAREBRACKET);
            }
            else if (PARSE(LBRACKET))
            {
                PARSE(dictorsetmaker);
                return PARSE(RBRACKET);
            }
            else if (PARSE(string_literal))
            {
                while (PARSE(string_literal)) {
                }
                return true;
            }
            return PARSE(IDENTIFIER) || PARSE(NUMBER) || PARSE(ELLIPSIS) ||
                   PARSE(NONE) || PARSE(TRUE) || PARSE(FALSE);
        }

        // testlist_comp: (test|star_expr) ( comp_for | (',' (test|star_expr))*
        // [','] )
        PARSER(testlist_comp)
        {
            if (!(PARSE(test) || PARSE(star_expr))) return false;
            while (true) {
                if (PARSE(comp_for)) continue;
                if (PARSE(COMMA)) {
                    if (!(PARSE(test) || PARSE(star_expr))) return false;
                    continue;
                }
                break;
            }
            return true;
        }

        // trailer: '(' [arglist] ')' | '[' subscriptlist ']' | '.' NAME
        PARSER(trailer)
        {
            if (PARSE(LPAR)) {
                PARSE(arglist);
                return PARSE(RPAR);
            }
            else if (PARSE(LSQUAREBRACKET))
            {
                return PARSE(subscriptlist) && PARSE(RSQUAREBRACKET);
            }
            return PARSE(DOT) && PARSE(IDENTIFIER);
        }

        // subscriptlist: subscript (',' subscript)* [',']
        PARSER(subscriptlist)
        {
            if (!PARSE(subscript)) return false;
            while (PARSE(COMMA)) {
                if (!PARSE(subscript)) break;
            }
            return true;
        }

        // subscript: test | [test] ':' [test] [sliceop]
        PARSER(subscript)
        {
            if (PARSE(test)) {
                if (!PARSE(COLON)) return true;
            }
            else
            {
                if (!PARSE(COLON)) return false;
            }
            PARSE(test);
            PARSE(sliceop);
            return true;
        }

        // sliceop: ':' [test]
        PARSER(sliceop)
        {
            if (PARSE(COLON)) {
                PARSE(test);
                return true;
            }
            return false;
        }

        // exprlist: (expr|star_expr) (',' (expr|star_expr))* [',']
        PARSER(exprlist)
        {
            if (!(PARSE(expr) || PARSE(star_expr))) return false;
            while (PARSE(COMMA)) {
                if (!(PARSE(expr) || PARSE(star_expr))) break;
            }
            return true;
        }

        // testlist: test (',' test)* [',']
        PARSER(testlist)
        {
            if (!PARSE(test)) return false;
            while (PARSE(COMMA))
                if (!PARSE(test)) break;
            return true;
        }

        // dictorsetmaker: ( ((test ':' test | '**' expr)
        //                   (comp_for | (',' (test ':' test | '**' expr))*
        //                   [',']))
        //                 |
        //                  ((test | star_expr)
        //                   (comp_for | (',' (test | star_expr))* [','])) )
        //
        PARSER(dictorsetmaker_first)
        {
            if (PARSE(test)) {
                if (!(PARSE(COLON) && PARSE(test))) return false;
            }
            else if (!(PARSE(POW) && PARSE(expr)))
                return false;
            if (PARSE(comp_for)) return true;
            while (PARSE(COMMA)) {
                if (PARSE(test)) {
                    if (!(PARSE(COLON) && PARSE(test))) return false;
                }
                else if (PARSE(POW))
                {
                    if (!PARSE(expr)) return false;
                }
                else
                    break;
            }
            return true;
        }

        PARSER(dictorsetmaker_second)
        {
            if (!(PARSE(test) || PARSE(star_expr))) return false;
            if (PARSE(comp_for)) return true;
            while (PARSE(COMMA)) {
                if (PARSE(test) || PARSE(star_expr)) continue;
                break;
            }
            return true;
        }

        PARSER(dictorsetmaker)
        {
            return PARSE(dictorsetmaker_first) || PARSE(dictorsetmaker_second);
        }

        // classdef: 'class' NAME ['(' [arglist] ')'] ':' suite
        PARSER(classdef)
        {
            if (!(PARSE(CLASS) && PARSE(IDENTIFIER))) return false;
            if (PARSE(LPAR)) {
                PARSE(arglist);
                if (!PARSE(RPAR)) return false;
            }
            return PARSE(COLON) && PARSE(suite);
        }

        // arglist: argument (',' argument)*  [',']
        PARSER(arglist)
        {
            if (!PARSE(argument)) return false;
            while (PARSE(COMMA))
                if (!PARSE(argument)) break;
            return true;
        }

        //
        //# The reason that keywords are test nodes instead of NAME is that
        // using NAME
        //# results in an ambiguity. ast.c makes sure it's a NAME.
        //# "test '=' test" is really "keyword '=' test", but we have no such
        // token.
        //# These need to be in a single rule to avoid grammar that is ambiguous
        //# to our LL(1) parser. Even though 'test' includes '*expr' in
        // star_expr,
        //# we explicitly match '*' here, too, to give it proper precedence.
        //# Illegal combinations and orderings are blocked in ast.c:
        //# multiple (test comp_for) arguements are blocked; keyword unpackings
        //# that precede iterable unpackings are blocked; etc.
        // argument: ( test [comp_for] |
        //            test '=' test |
        //            '**' test |
        //            '*' test )
        PARSER(argument)
        {
            if (PARSE(test)) {
                if (PARSE(EQ)) return PARSE(test);
                PARSE(comp_for);
                return true;
            }
            if (PARSE(POW) || PARSE(MUL)) return PARSE(test);
            return false;
        }

        // comp_iter: comp_for | comp_if
        PARSER(comp_iter) { return PARSE(comp_for) || PARSE(comp_if); };

        // comp_for: 'for' exprlist 'in' or_test [comp_iter]
        PARSER(comp_for)
        {
            if (PARSE(FOR) && PARSE(exprlist) && PARSE(IN) && PARSE(or_test)) {
                PARSE(comp_iter);
                return true;
            }
            return false;
        }

        // comp_if: 'if' test_nocond [comp_iter]
        PARSER(comp_if)
        {
            if (PARSE(IF) && PARSE(test_nocond)) {
                PARSE(comp_iter);
                return true;
            }
            return false;
        }

        //# not used in grammar, but may appear in "node" passed from Parser to
        // Compiler
        // encoding_decl: NAME
        PARSER(encoding_decl) { return PARSE(IDENTIFIER); }
        //
        // yield_expr: 'yield' [yield_arg]
        PARSER(yield_expr)
        {
            if (PARSE(YIELD)) {
                PARSE(yield_arg);
                return true;
            }
            return false;
        }

        // yield_arg: 'from' test | testlist
        PARSER(yield_arg)
        {
            if (PARSE(FROM)) return PARSE(test);
            return PARSE(testlist);
        }

        // single_input: NEWLINE | simple_stmt | compound_stmt NEWLINE
        PARSER(single_input)
        {
            return PARSE(NEWLINE) || PARSE(simple_stmt) ||
                   (PARSE(compound_stmt) && true); //PARSE(NEWLINE));
        }

        // file_input: (NEWLINE | stmt)* ENDMARKER
        PARSER(file_input)
        {
            while (PARSE(NEWLINE) || PARSE(stmt)) {
            }
            return PARSE(ENDMARKER);
        }

        // eval_input: testlist NEWLINE* ENDMARKER
        PARSER(eval_input)
        {
            if (!PARSE(testlist)) return false;
            while (PARSE(NEWLINE)) {
            }
            return PARSE(ENDMARKER);
        }
    };
#pragma GCC diagnostic pop

    Lexer::Lexer(Source& source, Mode mode)
        : _mode(mode), _source(source), _current(_source.begin())
    {}

    bool Lexer::parse(Stack& stack)
    {
        Context ctx;
        switch (_mode)
        {
        case Mode::file: return ctx.parse_file_input(stack, _current);
        case Mode::eval: return ctx.parse_eval_input(stack, _current);
        case Mode::single: return ctx.parse_single_input(stack, _current);
        }
        std::abort();
    }
}}
