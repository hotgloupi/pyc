import functools
import collections

from .token import Token

__all__ = ['lex_file_input']

def parser(method):
    @functools.wraps(method)
    def wrapper(self):
        loc = self._loc
        tokens = self._tokens[:]
        block_indents = self._block_indents[:]
        self._call_stack.append(method.__name__.strip('_'))
        #print (' > '.join(self._call_stack))
        res = method(self)
        #print (' < '.join(self._call_stack), "->", res)
        if not res:
            self._loc = loc
            self._tokens = tokens
            self._block_indents = block_indents
        self._call_stack.pop()
        return res
    return wrapper

def terminal_parser(method):
    parse_method = parser(method)
    @functools.wraps(method)
    def wrapper(self):
        start = self._loc
        if parse_method(self):
            token = Token[method.__name__.lstrip('_').upper()]
            string = self._data[start:self._loc]
            self._tokens.append(Lexeme(token, string, (start, self._loc)))
            if token != Token.NEWLINE:
                while self.char == ' ':
                    self.incr()
            return True
        return False
    return wrapper

def lex_file_input(data):
    lexer = Lexer(data)
    lexer._file_input()
    return lexer._tokens

Lexeme = collections.namedtuple('Lexeme', ('token', 'str', 'loc'))

class Lexer:
    def __init__(self, data):
        self._data = data
        self._tokens = []
        self._loc = 0
        self._block_indents = [0]
        self._call_stack = []

    def next(self, offset = 1):
        i = self._loc + offset
        if i >= len(self._data):
            return None
        return self._data[i]

    def _str(self, s):
        for i in range(len(s)):
            if s[i] != self.next(i): return False
        self._loc += len(s)
        return True

    @property
    def char(self):
        return self.next(0)

    def incr(self):
        self._loc += 1

    @terminal_parser
    def _newline(self):
        if self.char == '#':
            while self.char not in ('\n', None):
                self.incr()

        if self._str("\r") or self._str("\n"):
            # while (self._str("\r") or self._str("\n")) {}
            return True
        return False

    @terminal_parser
    def _identifier(self):
        start = self._loc
        while self.char is not None and (('a' <= self.char <= 'z') or ('A' <= self.char <= 'Z')):
            self.incr()
        return start != self._loc

    @terminal_parser
    def _indent(self):
        assert len(self._block_indents) > 0
        i = 0
        while self._str(" ") or self._str("\t"):
            i += 1
        if i < self._block_indents[-1]:
            #std::cout << "WRONG INDENT: expected >= "
            #          << block_indents.back() << " got " << i << std::endl
            return False
        self._block_indents.append(i)
        return True

    @terminal_parser
    def _dedent(self):
        assert len(self._block_indents) > 0
        i = 0
        while self._str(" ") or self._str("\t"):
            i += 1
        for indent in reversed(self._block_indents):
            if indent <= i:
                break
        if indent != i:
            #std::cout << "WRONG DEDENT: expected " << *it << " got " << i
            #          << std::endl
            return False
        self._block_indents.pop()
        return True

    @terminal_parser
    def _number(self):
        start = self._loc
        while self.char is not None and ('0' <= self.char <= '9'):
            self.incr()
        return start != self._loc

    @parser
    def _string_literal(self):
        bytes = False
        raw = False
        formatted = False
        unicode = False
        while True:
            if self.char in ('b', 'B'):
                bytes = True
                self.incr()
                continue
            if self.char in ('r', 'R'):
                raw = True
                self.incr()
                continue
            if self.char in ('u', 'U'):
                unicode = True
                self.incr()
                continue
            if self.char in ('f', 'F'):
                formatted = True
                self.incr()
                continue
            if self.char in ('"', '\''):
                break
            return False
        if bytes and formatted:
            self._throw()
        if bytes and unicode:
            self._throw()

        delimiter = self.char
        delim_size = 1
        if self.next(1) == delimiter and self.next(2) == delimiter:
            delim_size = 3
        self._loc += delim_size

        ignore_next = False
        start = self._loc
        while self.char is not None:
            if ignore_next:
                ignore_next = False
                self.incr()
                continue
            if self.char == '\\':
                ignore_next = True
                self.incr()
                continue
            end = self._loc
            i = 0
            while self.char == delimiter and i < delim_size:
                self.incr()
                i += 1
            if i == 0:
                self.incr()
            elif i == delim_size:
                if bytes:
                    token = raw and Token.RAW_BYTES or Token.BYTES
                elif formatted:
                    token = raw and Token.RAW_FORMATTED_STRING or Token.FORMATTED_STRING
                else:
                    token = raw and Token.RAW_UNICODE or Token.UNICODE
                self._tokens.append(Lexeme(token, self._data[start:end], (start, end)))
                return True
            elif delim_size == 1 and (self.char == '\r' or self.char == '\n'):
                self._throw()
        return False

    @terminal_parser
    def _eof(self):
        return self.char == None
    _endmarker = _eof

    @terminal_parser
    def _at(self):
        return self._str("@")

    @terminal_parser
    def _colon(self):
        return self._str(":")

    @terminal_parser
    def _comma(self):
        return self._str(",")

    @terminal_parser
    def _equal(self):
        return self._str("=")
    _eq = _equal

    @terminal_parser
    def _lpar(self):
        return self._str("(")

    @terminal_parser
    def _star(self):
        return self._str("*")

    @terminal_parser
    def _double_star(self):
        return self._str("**")

    @terminal_parser
    def _right_arrow(self):
        return self._str("->")
    _rarrow = _right_arrow

    @terminal_parser
    def _rpar(self):
        return self._str(")")

    @terminal_parser
    def _semicolon(self):
        return self._str("")

    @terminal_parser
    def _lsqb(self):
        return self._str("[")
    _lsquarebracket = _lsqb

    @terminal_parser
    def _rsqb(self):
        return self._str("]")
    _rsquarebracket = _rsqb

    @terminal_parser
    def _left_brace(self):
        return self._str("{")

    @terminal_parser
    def _right_brace(self):
        return self._str("}")

    @terminal_parser
    def _as(self):
        return self._str("as")

    @terminal_parser
    def _async(self):
        return self._str("async")

    @terminal_parser
    def _break(self):
        return self._str("break")

    @terminal_parser
    def _continue(self):
        return self._str("continue")

    @terminal_parser
    def _def(self):
        return self._str("def")

    @terminal_parser
    def _del(self):
        return self._str("del")

    @terminal_parser
    def _dot(self):
        return self._str(".")

    @terminal_parser
    def _ellipsis(self):
        return self._str("...")

    @terminal_parser
    def _from(self):
        return self._str("from")

    @terminal_parser
    def _import(self):
        return self._str("import")

    @terminal_parser
    def _pass(self):
        return self._str("pass")

    @terminal_parser
    def _raise(self):
        return self._str("raise")

    @terminal_parser
    def _return(self):
        return self._str("return")

    @terminal_parser
    def _global(self):
        return self._str("global")

    @terminal_parser
    def _nonlocal(self):
        return self._str("local")

    @terminal_parser
    def _assert(self):
        return self._str("assert")

    @terminal_parser
    def _if(self):
        return self._str("if")

    @terminal_parser
    def _elif(self):
        return self._str("elif")

    @terminal_parser
    def _else(self):
        return self._str("else")

    @terminal_parser
    def _while(self):
        return self._str("while")

    @terminal_parser
    def _for(self):
        return self._str("for")

    @terminal_parser
    def _try(self):
        return self._str("try")

    @terminal_parser
    def _finally(self):
        return self._str("finally")

    @terminal_parser
    def _with(self):
        return self._str("with")

    @terminal_parser
    def _except(self):
        return self._str("except")

    @terminal_parser
    def _lambda(self):
        return self._str("lambda")

    @terminal_parser
    def _or(self):
        return self._str("or")

    @terminal_parser
    def _and(self):
        return self._str("and")

    @terminal_parser
    def _not(self):
        return self._str("not")

    @terminal_parser
    def _await(self):
        return self._str("await")

    @terminal_parser
    def _class(self):
        return self._str("class")

    @terminal_parser
    def _yield(self):
        return self._str("yield")

    @terminal_parser
    def _pipe(self):
        return self._str("|")

    @terminal_parser
    def _circumflex(self):
        return self._str("^")

    @terminal_parser
    def _ampersand(self):
        return self._str("&")

    @terminal_parser
    def _right_shift(self):
        return self._str(">>")
    _rshift = _right_shift

    @terminal_parser
    def _left_shift(self):
        return self._str("<<")
    _lshift = _left_shift

    @terminal_parser
    def _plus(self):
        return self._str("+")

    @terminal_parser
    def _minus(self):
        return self._str("-")

    @terminal_parser
    def _star(self):
        return self._str("*")

    @terminal_parser
    def _at(self):
        return self._str("@")

    @terminal_parser
    def _slash(self):
        return self._str("/")

    @terminal_parser
    def _percent(self):
        return self._str("%")

    @terminal_parser
    def _double_slash(self):
        return self._str("//")

    @terminal_parser
    def _tilde(self):
        return self._str("~")

    @terminal_parser
    def _double_star(self):
        return self._str("**")

    @terminal_parser
    def _none(self):
        return self._str("None")

    @terminal_parser
    def _true(self):
        return self._str("True")

    @terminal_parser
    def _false(self):
        return self._str("False")

    # decorator: '@' dotted_name [ '(' [arglist] ')' ] NEWLINE
    @parser
    def _decorator(self):
        if not (self._at() and self._dotted_name()): return False
        if self._lpar():
            self._arglist()
            if not self._rpar(): return False
        return self._newline()

    # decorators: decorator+
    @parser
    def _decorators(self):
        if self._decorator():
            while self._decorator(): pass
            return True
        return False

    # decorated: decorators (classdef | funcdef | async_funcdef)
    @parser
    def _decorated(self):
        return self._decorators() and \
               (self._classdef() or self._funcdef() or self._async_funcdef())

    # async_funcdef: 'async' funcdef
    @parser
    def _async_funcdef(self):  return self._async() and self._funcdef()

    # funcdef: 'def' NAME parameters ['->' test] ':' suite
    @parser
    def _funcdef(self):
        if self._def() and self._identifier() and self._parameters():
            if self._rarrow() and not self._test(): return False
            return self._colon() and self._suite()
        return False

    # parameters: '(' [typedargslist] ')'
    @parser
    def _parameters(self):
        if self._lpar():
            self._typedargslist()
            return self._rpar()
        return False

    # typedargslist: (
    #      tfpdef ['=' test] (',' tfpdef ['=' test])*
    #      [
    #          ','
    #          [
    #              '*'
    #              [tfpdef]
    #              (',' tfpdef ['=' test])*
    #              [',' ['**' tfpdef [',']]]
    #                  | '**' tfpdef [',']
    #          ]
    #      ]
    #  | '*' [tfpdef] (',' tfpdef ['=' test])* [',' ['**' tfpdef [',']]]
    #  | '**' tfpdef [','])
    @parser
    def _typedargslist(self):
        if self._tfpdef():
            # tfpdef ['=' test] (',' tfpdef ['=' test])*
            if self._eq() and not self._test(): return False
            while self._comma():
                if not self._tfpdef(): return False
                if self._eq() and not self._test(): return False
            # [
            #     ','
            #     [
            #         '*'
            #         [tfpdef]
            #         (',' tfpdef ['=' test])*
            #         [',' ['**' tfpdef [',']]]
            #             | '**' tfpdef [',']
            #     ]
            # ]
            if self._comma():
                if self._star():
                    self._tfpdef()
                    while True:
                        if not self._comma(): break
                        if self._tfpdef():
                            # We parse (',' tfpdef ['=' test])*
                            if self._eq() and not self._test(): return False
                        else:
                            if self._double_star():
                                if not self._tfpdef(): return False
                                self._comma()
                            break
                elif self._double_star():
                    if not self._tfpdef(): return False
                    self._comma()
            return True
        elif self._star():
            #  | '*' [tfpdef] (',' tfpdef ['=' test])* [',' ['**' tfpdef
            #  [',']]]
            self._tfpdef()
            while True:
                if not self._comma(): break
                if self._tfpdef():
                    # We parse (',' tfpdef ['=' test])*
                    if self._eq() and not self._test(): return False
                else:
                    # [',' ['**' tfpdef [',']]]
                    if self._double_star():
                        if not self._tfpdef(): return False
                        self._comma()
                    break
            return True
        elif self._double_star():
            #  | '**' tfpdef [','])
            if not self._tfpdef(): return False
            self._comma()
            return True
        return False

    # tfpdef: NAME [':' test]
    @parser
    def _tfpdef(self):
        if self._identifier():
            if self._colon() and not self._test(): return False
            return True
        return False

    # varargslist: (
    # 1      vfpdef ['=' test]
    # 2      (',' vfpdef ['=' test])*
    # 3      [
    # 4          ','
    # 5          [
    # 6              '*'
    # 7              [vfpdef]
    # 8              (',' vfpdef ['=' test])*
    # 9              [',' ['**' vfpdef [',']]]
    # 10             | '**' vfpdef [',']
    # 11         ]
    # 12     ]
    # 13     | '*' [vfpdef] (',' vfpdef ['=' test])* [',' ['**' vfpdef
    # [',']]]
    # 14     | '**' vfpdef [',']
    # )
    @parser
    def _varargslist(self):
        if self._vfpdef():  # 1
            if self._eq() and not self._test():  # 1
                return False
            while True:
                if not self._comma(): break  # 2
                if self._vfpdef():         # 2
                    if self._eq() and not self._test(): return False
                else:  # 5
                    if self._star():  # 6
                        self._vfpdef()  # 7
                        while True:
                            if not self._comma(): break  # 8
                            if self._vfpdef():         # 8
                                if self._eq() and not self._test(): return False
                            else:  # 9
                                if self._star():
                                    if not self._vfpdef(): return False
                                    self._comma()
                                break
                    elif self._double_star():  # 10
                        if not self._vfpdef(): return False
                        self._comma()
                    break
            return True
        elif self._star():
            # 13     | '*' [vfpdef] (',' vfpdef ['=' test])* [',' ['**'
            # vfpdef [',']]]
            self._vfpdef()
            while True:
                if not self._comma(): break
                if self._vfpdef():
                    if self._eq() and not self._test(): return False
                else:
                    if self._double_star():
                        if not self._vfpdef(): return False
                        self._comma()
                    break
            return True
        elif self._double_star():
            # 14     | '**' vfpdef [',']
            if not self._vfpdef(): return False
            self._comma()
            return True
        return False

    # vfpdef: NAME
    @parser
    def _vfpdef(self): return self._identifier()

    # stmt: simple_stmt | compound_stmt
    @parser
    def _stmt(self): return self._simple_stmt() or self._compound_stmt()

    # simple_stmt: small_stmt ('' small_stmt)* [''] NEWLINE
    @parser
    def _simple_stmt(self):
        if not self._small_stmt(): return False
        while self._semicolon():
            if not self._small_stmt(): break
        return self._newline()

    # small_stmt: (expr_stmt | del_stmt | pass_stmt | flow_stmt |
    #             import_stmt | global_stmt | nonlocal_stmt | assert_stmt)
    @parser
    def _small_stmt(self):
        return self._del_stmt() or self._pass_stmt() or \
               self._flow_stmt() or self._import_stmt() or \
               self._global_stmt() or self._nonlocal_stmt() or \
               self._assert_stmt() or self._expr_stmt()

    # expr_stmt: testlist_star_expr (augassign (yield_expr|testlist) |
    #                     ('=' (yield_expr|testlist_star_expr))*)
    @parser
    def _expr_stmt(self):
        if not self._testlist_star_expr(): return False
        if self._augassign(): return self._yield_expr() or self._testlist()
        while self._eq():
            if not (self._yield_expr() or self._testlist()):
                return False
        return True

    # testlist_star_expr: (test|star_expr) (',' (test|star_expr))* [',']
    @parser
    def _testlist_star_expr(self):
        if not (self._test() or self._star_expr()): return False
        while self._comma():
            if not (self._test() or self._star_expr()): break
        return True

    @terminal_parser
    def _plus_equal(self):
        return self._str("+=")

    @terminal_parser
    def _minus_equal(self):
        return self._str("-=")

    @terminal_parser
    def _star_equal(self):
        return self._str("*=")

    @terminal_parser
    def _slash_equal(self):
        return self._str("/=")

    @terminal_parser
    def _percent_equal(self):
        return self._str("%=")

    @terminal_parser
    def _ampersand_equal(self):
        return self._str("&=")

    @terminal_parser
    def _pipe_equal(self):
        return self._str("|=")

    @terminal_parser
    def _circumflex_equal(self):
        return self._str("^=")

    @terminal_parser
    def _left_shift_equal(self):
        return self._str("<<=")

    @terminal_parser
    def _right_shift_equal(self):
        return self._str(">>=")

    @terminal_parser
    def _double_star_equal(self):
        return self._str("**=")

    @terminal_parser
    def _double_slash_equal(self):
        return self._str("#=")

    @terminal_parser
    def _at_equal(self):
        return self._str("@=")

    # augassign: ('+=' | '-=' | '*=' | '@=' | '/=' | '%=' | '&=' | '|=' |
    # '^=' |
    #            '<<=' | '>>=' | '**=' | '#=')
    @parser
    def _augassign(self):
        return self._plus_equal() or self._minus_equal() or \
               self._star_equal() or self._at_equal() or self._slash_equal() or \
               self._percent_equal() or self._ampersand_equal() or \
               self._pipe_equal() or self._circumflex_equal() or \
               self._left_shift_equal() or self._right_shift_equal() or \
               self._double_star_equal() or self._double_slash_equal()

    ## For normal assignments, additional restrictions enforced by the
    # interpreter
    # del_stmt: 'del' exprlist
    @parser
    def _del_stmt(self):  return self._del() and self._exprlist()

    # pass_stmt: 'pass'
    @parser
    def _pass_stmt(self):  return self._pass()

    # flow_stmt: break_stmt | continue_stmt | return_stmt | raise_stmt |
    # yield_stmt
    @parser
    def _flow_stmt(self):
        return self._break_stmt() or self._continue_stmt() or \
               self._return_stmt() or self._raise_stmt() or self._yield_stmt()

    # break_stmt: 'break'
    @parser
    def _break_stmt(self):  return self._break()

    # continue_stmt: 'continue'
    @parser
    def _continue_stmt(self):  return self._continue()

    # return_stmt: 'return' [testlist]
    @parser
    def _return_stmt(self):
        if not self._return(): return False
        self._testlist()
        return True

    # yield_stmt: yield_expr
    @parser
    def _yield_stmt(self):  return self._yield_expr()

    # raise_stmt: 'raise' [test ['from' test]]
    @parser
    def _raise_stmt(self):
        if not self._raise(): return False
        if self._test():
            if self._from() and not self._test(): return False
        return True

    # import_stmt: import_name | import_from
    @parser
    def _import_stmt(self):  return self._import_name() or self._import_from()

    # import_name: 'import' dotted_as_names
    @parser
    def _import_name(self):  return self._import() and self._dotted_as_names()

    ## note below: the ('.' | '...') is necessary because '...' is
    # tokenized as
    # ELLIPSIS
    # import_from: 'from' (('.' | '...')* dotted_name | ('.' | '...')+)
    #              'import' ('*' | '(' import_as_names ')' |
    #              import_as_names)
    @parser
    def _import_from(self):
        if not self._from(): return False
        has_dot = False
        while self._dot() or self._ellipsis():
            has_dot = True
        if not self._dotted_name():
            if not has_dot: return False
        return self._import() and \
               (self._star() or
                (self._lpar() and self._import_as_names() and self._rpar()) or
                self._import_as_names())

    # import_as_name: NAME ['as' NAME]
    @parser
    def _import_as_name(self):
        if not self._identifier(): return False
        if self._as() and not self._identifier(): return False
        return True

    # dotted_as_name: dotted_name ['as' NAME]
    @parser
    def _dotted_as_name(self):
        if not self._dotted_name(): return False
        if self._as() and not self._identifier(): return False
        return True

    # import_as_names: import_as_name (',' import_as_name)* [',']
    @parser
    def _import_as_names(self):
        if not self._import_as_name(): return False
        while self._comma():
            if not self._import_as_name(): break
        return True

    # dotted_as_names: dotted_as_name (',' dotted_as_name)*
    @parser
    def _dotted_as_names(self):
        if not self._dotted_as_name(): return False
        while self._comma():
            if not self._dotted_as_name(): return False
        return True

    # dotted_name: NAME ('.' NAME)*
    @parser
    def _dotted_name(self):
        if self._import(): return False
        if not self._identifier(): return False
        while self._dot():
            if not self._identifier(): return False
        return True

    # global_stmt: 'global' NAME (',' NAME)*
    @parser
    def _global_stmt(self):
        if not (self._global() and self._identifier()): return False
        while self._comma():
            if not self._identifier(): return False
        return True

    # nonlocal_stmt: 'nonlocal' NAME (',' NAME)*
    @parser
    def _nonlocal_stmt(self):
        if not (self._nonlocal() and self._identifier()): return False
        while self._comma():
            if not self._identifier(): return False
        return True

    # assert_stmt: 'assert' test [',' test]
    @parser
    def _assert_stmt(self):
        if not (self._assert() and self._test()): return False
        while self._comma():
            if not self._test(): return False
        return True

    #
    # compound_stmt: if_stmt | while_stmt | for_stmt | try_stmt | with_stmt
    # |
    # funcdef | classdef | decorated | async_stmt
    @parser
    def _compound_stmt(self):
        return self._if_stmt() or self._while_stmt() or self._for_stmt() or \
               self._try_stmt() or self._with_stmt() or self._funcdef() or \
               self._classdef() or self._decorated() or self._async_stmt()

    # async_stmt: ASYNC (funcdef | with_stmt | for_stmt)
    @parser
    def _async_stmt(self):
        return self._async() and \
               (self._funcdef() or self._with_stmt() or self._for_stmt())

    # if_stmt: 'if' test ':' suite ('elif' test ':' suite)* ['else' ':'
    # suite]
    @parser
    def _if_stmt(self):
        if not (self._if() and self._test() and self._colon() and self._suite()):
            return False
        while self._elif():
            if not (self._test() and self._colon() and self._suite()):
                return False
        if self._else(): return self._colon() and self._suite()
        return True

    # while_stmt: 'while' test ':' suite ['else' ':' suite]
    @parser
    def _while_stmt(self):
        if not (self._while() and self._test() and self._colon() and self._suite()):
            return False
        if self._else(): return self._colon() and self._suite()
        return True

    # for_stmt: 'for' exprlist 'in' testlist ':' suite ['else' ':' suite]
    @parser
    def _for_stmt(self):
        if not (self._for() and self._exprlist() and self._in() and
                self._testlist() and self._colon() and self._suite()):
            return False
        if self._else(): return self._colon() and self._suite()
        return True

    # try_stmt: 'try' ':' suite
    #           ((except_clause ':' suite)+
    #            ['else' ':' suite]
    #            ['finally' ':' suite] |
    #           'finally' ':' suite)
    @parser
    def _try_stmt(self):
        if not (self._try() and self._colon() and self._suite()): return False
        if self._except_clause():
            while True:
                if not (self._colon() and self._suite()): return False
                if not self._except_clause(): break

            if self._else():
                if not (self._colon() and self._suite()): return False
            if self._finally():
                if not (self._colon() and self._suite()): return False
        else:
            return self._finally() and self._colon() and self._suite()
        return True

    # with_stmt: 'with' with_item (',' with_item)*  ':' suite
    @parser
    def _with_stmt(self):
        if not (self._with() and self._with_item()): return False
        while self._comma():
            if not self._with_item(): return False
        return self._colon() and self._suite()

    # with_item: test ['as' expr]
    @parser
    def _with_item(self):
        if not self._test(): return False
        if self._as(): return self._expr()
        return True

    ## NB compile.c makes sure that the default except clause is last
    # except_clause: 'except' [test ['as' NAME]]
    @parser
    def _except_clause(self):
        if not self._except(): return False
        if self._test():
            if self._as(): return self._identifier()
        return True

    # suite: simple_stmt | NEWLINE INDENT stmt+ DEDENT
    @parser
    def _suite(self):
        if self._simple_stmt(): return True
        if not (self._newline() and self._indent() and self._stmt()): return False
        while self._stmt(): pass
        return self._dedent()

    # test: or_test ['if' or_test 'else' test] | lambdef
    @parser
    def _test(self):
        if self._or_test():
            if self._if():
                if not (self._or_test() and self._else() and self._test()):
                    return False
            return True
        return self._lambdef()

    # test_nocond: or_test | lambdef_nocond
    @parser
    def _test_nocond(self):  return self._or_test() or self._lambdef_nocond()

    # lambdef: 'lambda' [varargslist] ':' test
    @parser
    def _lambdef(self):
        if not self._lambda(): return False
        self._varargslist()
        return self._colon() and self._test()

    # lambdef_nocond: 'lambda' [varargslist] ':' test_nocond
    @parser
    def _lambdef_nocond(self):
        if not self._lambda(): return False
        self._varargslist()
        return self._colon() and self._test_nocond()

    # or_test: and_test ('or' and_test)*
    @parser
    def _or_test(self):
        if not self._and_test(): return False
        while self._or():
            if not self._and_test(): return False
        return True

    # and_test: not_test ('and' not_test)*
    @parser
    def _and_test(self):
        if not self._not_test(): return False
        while self._and():
            if not self._not_test(): return False
        return True

    # not_test: 'not' not_test | comparison
    @parser
    def _not_test(self):
        if self._not(): return self._not_test()
        return self._comparison()

    # comparison: expr (comp_op expr)*
    @parser
    def _comparison(self):
        if not self._expr(): return False
        while self._comp_op():
            if not self._expr(): return False
        return True

    @terminal_parser
    def _less(self):
        return self._str("<")

    @terminal_parser
    def _greater(self):
        return self._str(">")

    @terminal_parser
    def _double_equal(self):
        return self._str("==")

    @terminal_parser
    def _not_equal(self):
        return self._str("!=")

    @terminal_parser
    def _less_equal(self):
        return self._str("<=")

    @terminal_parser
    def _greater_equal(self):
        return self._str(">=")

    @terminal_parser
    def _in(self):
        return self._str("in")

    @terminal_parser
    def _not_in(self):
        return self._str("not") and self._str("in")

    @terminal_parser
    def _is(self):
        return self._str("is")

    @terminal_parser
    def _is_not(self):
        return self._str("is") and self._str("not")

    ## <> isn't actually a valid comparison operator in Python. It's here
    # for the
    ## sake of a __future__ import described in PEP 401 (which really works
    #:-)
    # comp_op: '<'|'>'|'=='|'>='|'<='|'<>'|'!='|'in'|'not' 'in'|'is'|'is'
    # 'not'
    @parser
    def _comp_op(self):
        return self._less() or self._greater() or self._double_equal() or \
               self._greater_equal() or self._less_equal() or \
               self._not_equal() or self._in() or self._not_in() or self._is() or \
               self._is_not()

    # star_expr: '*' expr
    @parser
    def _star_expr(self):  return self._star() and self._expr()

    # expr: xor_expr ('|' xor_expr)*
    @parser
    def _expr(self):
        if not self._xor_expr(): return False
        while self._or():
            if not self._xor_expr(): return False
        return True

    # xor_expr: and_expr ('^' and_expr)*
    @parser
    def _xor_expr(self):
        if not self._and_expr(): return False
        while self._circumflex():
            if not self._and_expr(): return False
        return True

    # and_expr: shift_expr ('&' shift_expr)*
    @parser
    def _and_expr(self):
        if not self._shift_expr(): return False
        while self._and():
            if not self._shift_expr(): return False
        return True

    # shift_expr: arith_expr (('<<'|'>>') arith_expr)*
    @parser
    def _shift_expr(self):
        if not self._arith_expr(): return False
        while self._lshift() or self._rshift():
            if not self._arith_expr(): return False
        return True

    # arith_expr: term (('+'|'-') term)*
    @parser
    def _arith_expr(self):
        if not self._term(): return False
        while self._plus() or self._minus():
            if not self._term(): return False
        return True

    # term: factor (('*'|'@'|'/'|'%'|'#') factor)*
    @parser
    def _term(self):
        if not self._factor(): return False
        while self._star() or self._at() or self._slash() or \
              self._percent() or self._double_slash():
            if not self._factor(): return False
        return True

    # factor: ('+'|'-'|'~') factor | power
    @parser
    def _factor(self):
        if self._plus() or self._minus() or self._not():
            return self._factor()
        return self._power()

    # power: atom_expr ['**' factor]
    @parser
    def _power(self):
        if not self._atom_expr(): return False
        if self._double_star(): return self._factor()
        return True

    # atom_expr: [AWAIT] atom trailer*
    @parser
    def _atom_expr(self):
        self._await()
        if not self._atom(): return False
        while self._trailer(): pass
        return True

    # atom: ('(' [yield_expr|testlist_comp] ')' |
    #       '[' [testlist_comp] ']' |
    #       '{' [dictorsetmaker] '}' |
    #       NAME | NUMBER | STRING+ | '...' | 'None' | 'True' | 'False')
    @parser
    def _atom(self):
        if self._lpar():
            if not self._yield_expr() or not self._testlist_comp():
                return false
            return self._rpar()
        elif self._lsquarebracket():
            self._testlist_comp()
            return self._rsquarebracket()
        elif self._left_brace():
            self._dictorsetmaker()
            return self._right_brace()
        elif self._string_literal():
            while self._string_literal(): pass
            return True
        return self._identifier() or self._number() or self._ellipsis() or \
               self._none() or self._true() or self._false()

    # testlist_comp: (test|star_expr) ( comp_for | (',' (test|star_expr))*
    # [','] )
    @parser
    def _testlist_comp(self):
        if not (self._test() or self._star_expr()): return False
        while True:
            if self._comp_for(): continue
            if self._comma():
                if not (self._test() or self._star_expr()): return False
                continue
            break
        return True

    # trailer: '(' [arglist] ')' | '[' subscriptlist ']' | '.' NAME
    @parser
    def _trailer(self):
        if self._lpar():
            self._arglist()
            return self._rpar()
        elif self._lsquarebracket():
            return self._subscriptlist() and self._rsquarebracket()
        return self._dot() and self._identifier()

    # subscriptlist: subscript (',' subscript)* [',']
    @parser
    def _subscriptlist(self):
        if not self._subscript(): return False
        while self._comma():
            if not self._subscript(): break
        return True

    # subscript: test | [test] ':' [test] [sliceop]
    @parser
    def _subscript(self):
        if self._test():
            if not self._colon(): return True
        else:
            if not self._colon(): return False
        self._test()
        self._sliceop()
        return True

    # sliceop: ':' [test]
    @parser
    def _sliceop(self):
        if self._colon():
            self._test()
            return True
        return False

    # exprlist: (expr|star_expr) (',' (expr|star_expr))* [',']
    @parser
    def _exprlist(self):
        if not (self._expr() or self._star_expr()): return False
        while self._comma():
            if not (self._expr() or self._star_expr()): break
        return True

    # testlist: test (',' test)* [',']
    @parser
    def _testlist(self):
        if not self._test(): return False
        while self._comma():
            if not self._test(): break
        return True

    # dictorsetmaker: ( ((test ':' test | '**' expr)
    #                   (comp_for | (',' (test ':' test | '**' expr))*
    #                   [',']))
    #                 |
    #                  ((test | star_expr)
    #                   (comp_for | (',' (test | star_expr))* [','])) )
    #
    @parser
    def _dictorsetmaker_first(self):
        if self._test():
            if not (self._colon() and self._test()): return False
        elif not (self._double_star() and self._expr()):
            return False
        if self._comp_for(): return True
        while self._comma():
            if self._test():
                if not (self._colon() and self._test()): return False
            elif self._double_star():
                if not self._expr(): return False
            else:
                break
        return True

    @parser
    def _dictorsetmaker_second(self):
        if not (self._test() or self._star_expr()): return False
        if self._comp_for(): return True
        while self._comma():
            if self._test() or self._star_expr(): continue
            break
        return True

    @parser
    def _dictorsetmaker(self):
        return self._dictorsetmaker_first() or self._dictorsetmaker_second()

    # classdef: 'class' NAME ['(' [arglist] ')'] ':' suite
    @parser
    def _classdef(self):
        if not (self._class() and self._identifier()): return False
        if self._lpar():
            self._arglist()
            if not self._rpar(): return False
        return self._colon() and self._suite()

    # arglist: argument (',' argument)*  [',']
    @parser
    def _arglist(self):
        if not self._argument(): return False
        while self._comma():
            if not self._argument(): break
        return True

    #
    ## The reason that keywords are test nodes instead of NAME is that
    # using NAME
    ## results in an ambiguity. ast.c makes sure it's a NAME.
    ## "test '=' test" is really "keyword '=' test", but we have no such
    # token.
    ## These need to be in a single rule to avoid grammar that is ambiguous
    ## to our LL(1) parser. Even though 'test' includes '*expr' in
    # star_expr,
    ## we explicitly match '*' here, too, to give it proper precedence.
    ## Illegal combinations and orderings are blocked in ast.c:
    ## multiple (test comp_for) arguements are blocked keyword unpackings
    ## that precede iterable unpackings are blocked etc.
    # argument: ( test [comp_for] |
    #            test '=' test |
    #            '**' test |
    #            '*' test )
    @parser
    def _argument(self):
        if self._test():
            if self._eq(): return self._test()
            self._comp_for()
            return True
        if self._double_star() or self._star(): return self._test()
        return False

    # comp_iter: comp_for | comp_if
    @parser
    def _comp_iter(self):  return self._comp_for() or self._comp_if()

    # comp_for: 'for' exprlist 'in' or_test [comp_iter]
    @parser
    def _comp_for(self):
        if self._for() and self._exprlist() and self._in() and self._or_test():
            self._comp_iter()
            return True
        return False

    # comp_if: 'if' test_nocond [comp_iter]
    @parser
    def _comp_if(self):
        if self._if() and self._test_nocond():
            self._comp_iter()
            return True
        return False

    ## not used in grammar, but may appear in "node" passed from Parser to
    # Compiler
    # encoding_decl: NAME
    @parser
    def _encoding_decl(self):  return self._identifier()
    #
    # yield_expr: 'yield' [yield_arg]
    @parser
    def _yield_expr(self):
        if self._yield():
            self._yield_arg()
            return True
        return False

    # yield_arg: 'from' test | testlist
    @parser
    def _yield_arg(self):
        if self._from(): return self._test()
        return self._testlist()

    # single_input: NEWLINE | simple_stmt | compound_stmt NEWLINE
    @parser
    def _single_input(self):
        return self._newline() or self._simple_stmt() or \
               (self._compound_stmt() and true) #self._newline())

    # file_input: (NEWLINE | stmt)* ENDMARKER
    @parser
    def _file_input(self):
        while self._newline() or self._stmt(): pass
        return self._endmarker()

    # eval_input: testlist NEWLINE* ENDMARKER
    @parser
    def _eval_input(self):
        if not self._testlist(): return False
        while self._newline(): pass
        return self._endmarker()
