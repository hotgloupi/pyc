from .. import ast
from .token import Token

__all__ = ['parse']

def parse(tokens, file = '<unknown>'):
    parser = Parser(tokens, file)
    return parser.parse()


operator_precedence = {
    Token.OR: 10,
    Token.AND: 20,
    #Token.NOT: 30,
    Token.LESS: 40,
    Token.LESS_EQUAL: 40,
    Token.GREATER: 40,
    Token.GREATER_EQUAL: 40,
    Token.EQUAL_EQUAL: 40,
    Token.NOT_EQUAL: 40,
    Token.IN: 40,
    Token.NOT_IN: 40,
    Token.IS: 40,
    Token.IS_NOT: 40,
    Token.PIPE: 50,
    Token.CIRCUMFLEX: 60,
    Token.AMPERSAND: 70,
    Token.LEFT_SHIFT: 80,
    Token.RIGHT_SHIFT: 80,
    Token.PLUS: 90,
    Token.MINUS: 90,
    Token.STAR: 100,
    Token.AT: 100,
    Token.SLASH: 100,
    Token.PERCENT: 100,
    Token.DOUBLE_SLASH: 100,
    Token.DOUBLE_STAR: 110,
}

class Parser:
    def __init__(self, tokens, file):
        self._tokens = tokens
        self._loc = 0
        self._end = len(tokens)

    def parse(self):
        return self._block()

    @property
    def eof(self):
        return self._loc >= self._end

    @property
    def tok(self):
        if self.eof:
            self._throw("Unexpected end of stream")
        return self._tokens[self._loc].token

    @property
    def loc(self):
        return self._tokens[self._loc].loc

    @property
    def str(self):
        if self.eof:
            self._throw("Unexpected end of stream")
        return self._tokens[self._loc].str

    def eat(self, *tokens):
        """ When a tokens are provided, returns True and advance if one of them
        matches the current token, otherwise returns False.
        If no token are provided, advance and return the eaten token.
        """
        if not tokens:
            token = self.tok
            self._loc += 1
            return token

        if self.tok in tokens:
            self._loc += 1
            return True
        return False

    def consume(self, token):
        if self.tok != token:
            self._throw("Expected %s, got %s" % (token, self.tok))
        str = self.str
        self._loc += 1
        return str

    def eat_newlines(self):
        found = False
        while self.eat(Token.NEWLINE, Token.SEMICOLON):
            found = True
        return found

    def _block(self):
        block = []
        while True:
            self.eat_newlines()
            if self.tok in (Token.EOF, Token.DEDENT):
                break
            block.append(self._stmt())
        return ast.Block(self.loc, block)

    def _indented_block(self):
        if not self.eat_newlines():
            # One line block
            return [self._stmt()]
        self.consume(Token.INDENT)
        res = self._block()
        if self.tok != Token.EOF:
            self.consume(Token.DEDENT)
        return res;

    def _stmt(self):
        if self.eat(Token.DEL):
            return ast.DelStatement(self.loc, self._expression_list())
        elif self.eat(Token.PASS):
            return ast.PassStatement(self.loc, )
        elif self.eat(Token.BREAK):
            return ast.BreakStatement(self.loc, )
        elif self.eat(Token.CONTINUE):
            return ast.ContinueStatement(self.loc, )
        elif self.eat(Token.RETURN):
            res = ast.ReturnStatement(self.loc, )
            if not self.eat_newlines():
                res.value = self._expression_list()
            return res
        elif self.eat(Token.RAISE):
            NOT_IMPLEMENTED()
            return ast.RaiseStatement(self.loc, )
        elif self.eat(Token.YIELD):
            NOT_IMPLEMENTED()
            return ast.YieldStatement(self.loc, )
        elif self.tok in (Token.IMPORT, Token.FROM):
            return self._import_stmt()
        elif self.eat(Token.GLOBAL):
            NOT_IMPLEMENTED()
            return ast.GlobalStatement(self.loc, )
        elif self.eat(Token.NONLOCAL):
            NOT_IMPLEMENTED()
            return ast.NonLocalStatement(self.loc, )
        elif self.eat(Token.ASSERT):
            NOT_IMPLEMENTED()
            return ast.AssertStatement(self.loc, )
        elif self.eat(Token.IF):
            NOT_IMPLEMENTED()
            return ast.IfStatement(self.loc, )
        elif self.eat(Token.WHILE):
            NOT_IMPLEMENTED()
            return ast.WhileStatement(self.loc, )
        elif self.eat(Token.FOR):
            NOT_IMPLEMENTED()
            return ast.ForStatement(self.loc, )
        elif self.eat(Token.TRY):
            NOT_IMPLEMENTED()
            return ast.TryStatement(self.loc, )
        elif self.eat(Token.WITH):
            NOT_IMPLEMENTED()
            return ast.WithStatement(self.loc, )
        elif self.eat(Token.DEF):
            name = self.consume(Token.IDENTIFIER)
            self.consume(Token.LEFT_PARENTHESIS)
            args = self._args()
            self.consume(Token.RIGHT_PARENTHESIS)
            self.consume(Token.COLON)
            return ast.FunctionDefinition(
                self.loc,
                name,
                args,
                self._indented_block()
            )
        elif self.eat(Token.CLASS):
            NOT_IMPLEMENTED()
            return ast.ClassDefinition(self.loc, )
        elif self.eat(Token.AT):
            NOT_IMPLEMENTED()
            return ast.DecoratedDefinition(self.loc, )
        elif self.eat(Token.ASYNC):
            NOT_IMPLEMENTED()
            return ast.AsyncDefinition(self.loc, )

        # Everything failed, we can try to read an "expr_stmt"
        # expr_stmt: testlist_star_expr (augassign (yield_expr|testlist) |
        #                      ('=' (yield_expr|testlist_star_expr))*)
        # testlist_star_expr: (test|star_expr) (',' (test|star_expr))* [',']
        # augassign: ('+=' | '-=' | '*=' | '@=' | '/=' | '%=' | '&=' |
        #             '|=' | '^=' | '<<=' | '>>=' | '**=' | '//=')

        lhs = self._expression_list()
        if self.tok in (Token.NEWLINE, Token.SEMICOLON):
            return ast.ExpressionStatement(self.loc, lhs)
        elif self.tok in (Token.EQUAL,
                            Token.PLUS_EQUAL,
                            Token.MINUS_EQUAL,
                            Token.STAR_EQUAL,
                            Token.AT_EQUAL,
                            Token.SLASH_EQUAL,
                            Token.PERCENT_EQUAL,
                            Token.AMPERSAND_EQUAL,
                            Token.PIPE_EQUAL,
                            Token.NOT_EQUAL,
                            Token.LEFT_SHIFT_EQUAL,
                            Token.RIGHT_SHIFT_EQUAL,
                            Token.DOUBLE_STAR_EQUAL,
                            Token.DOUBLE_SLASH_EQUAL
                         ):
            token = self.eat()
            rhs = self._expression_list()
            return ast.Assignment(
                self.loc,
                op = token,
                lhs = lhs,
                rhs = rhs
            )
        self._throw("Invalid token: %s \"%s\"" % (self.tok, self.str))

    # import_stmt: import_name | import_from
    # import_name: 'import' dotted_as_names
    # import_from: ('from' (('.' | '...')* dotted_name | ('.' | '...')+)
    #               'import' ('*' | '(' import_as_names ')' | import_as_names))
    # import_as_name: NAME ['as' NAME]
    # dotted_as_name: dotted_name ['as' NAME]
    # import_as_names: import_as_name (',' import_as_name)* [',']
    # dotted_as_names: dotted_as_name (',' dotted_as_name)*
    # dotted_name: NAME ('.' NAME)*
    def _import_stmt(self):
        from_ = []
        ImportStyle = ast.ImportStatement.Style
        style = ImportStyle.absolute
        if self.eat(Token.FROM):
            if self.eat(Token.DOT):
                if self.eat(Token.DOT):
                    style = ImportStyle.relative_to_parent
                else:
                    style = ImportStyle.relative_to_current
            from_ = self._dotted_name()
        self.consume(Token.IMPORT)
        imports = []
        while True:
            import_as = ast.ImportStatement.Import()
            import_as.dotted_name = self._dotted_name()
            if self.eat(Token.AS):
                import_as.rename = self.str
                self.eat()
            imports.append(import_as)
            self.eat(Token.COMMA)
            if self.eat_newlines():
                break
        return ast.ImportStatement(self.loc, style, from_, imports)

    def _dotted_name(self):
        res = []
        while True:
            if self.eat(Token.DOT):
                continue
            elif self.tok == Token.IDENTIFIER:
                res.append(self.str)
                self.eat()
            else:
                break
        return res;

    def _slice(self):
        from_ = self._expression_atom();
        to = None
        step = None
        if self.eat(Token.COLON):
            to = self._expression_atom();
            if self.eat(Token.COLON):
                step = self._expression_atom();
        return ast.Slice(self.loc, from_, to, step);

    def _expression_atom(self):
        if self.tok in (Token.PLUS, Token.MINUS, Token.TILDE):
            #return ast.UnaryExpression(self.eat(), )
            NOT_IMPLEMENTED()

        if self.eat(Token.LEFT_PARENTHESIS):
            res = self._expression_list()
            self.eat_newlines()
            self.consume(Token.RIGHT_PARENTHESIS)
        elif self.eat(Token.LAMBDA):
            # XXX parse lambda
            NOT_IMPLEMENTED()
        elif self.tok == Token.IDENTIFIER:
            res = ast.Identifier(self.loc, self.str)
            self.eat()
        elif self.tok == Token.UNICODE:
            res = ast.String(self.loc, self.str)
            self.eat()
        elif self.tok == Token.RAW_UNICODE:
            NOT_IMPLEMENTED()
        elif self.tok == Token.BYTES:
            NOT_IMPLEMENTED()
        elif self.tok == Token.RAW_BYTES:
            NOT_IMPLEMENTED()
        elif self.tok == Token.FORMATTED_STRING:
            NOT_IMPLEMENTED()
        elif self.tok == Token.RAW_FORMATTED_STRING:
            NOT_IMPLEMENTED()
        elif self.tok == Token.NUMBER:
            res = ast.Number(self.loc, self.str)
            self.eat()
        elif self.tok == Token.ELLIPSIS:
            NOT_IMPLEMENTED()
        else:
            NOT_IMPLEMENTED()

        while True:
            if self.eat(Token.LEFT_PARENTHESIS):
                args = self._args()
                self.consume(Token.RIGHT_PARENTHESIS)
                res = ast.FunctionCall(self.loc, res, args)
            elif self.eat(Token.LEFT_SQUARE_BRACKET):
                slice = _slice()
                self.consume(Token.RIGHT_SQUARE_BRACKET)
                res = ast.GetItem(self.loc, res, slice)
            elif self.eat(Token.DOT):
                NOT_IMPLEMENTED()
            else:
                break
        return res


    def _args(self):
        values = []
        while self.tok != Token.RIGHT_PARENTHESIS:
            expr = self._expression_atom();
            if self.eat(Token.EQUAL):
                if not isinstance(expr, ast.Identifier):
                    self._throw("Expected an identifier")

                values.append(
                    ast.NamedArgument(self.loc, expr.name, self._expression_atom())
                )
            else:
                values.append(expr)
            if not self.eat(Token.COMMA):
                break;
        return ast.ExpressionList(self.loc, values);

    # The operator precedence or 0
    def _precedence(self, op):
        if self.eof:
            # XXX why this check?
            return 0;
        return operator_precedence.get(op, 0);

    def _expression(self, last_precedence = -1, lhs = None):
        if lhs is None:
            lhs = self._expression_atom();

        while True:
            precedence = self._precedence(self.tok);
            if precedence == 0:
                if self.eat(Token.FOR):
                    # XXX Comprehension list here
                    NOT_IMPLEMENTED();
                return lhs;
            if precedence < last_precedence:
                return lhs;

            op = self.eat();
            rhs = self._expression_atom();
            next_precedence = self._precedence(self.tok);
            if next_precedence > precedence:
                rhs = self._expression(precedence + 1, rhs);
            lhs = ast.BinaryExpression(self.loc, op, lhs, rhs);


    #// Parse an expression or a list of expression
    #// Covers testlist and exprlist
    def _expression_list(self):
        list = None
        current_expression = None;
        while True:
            if self.tok == Token.NEWLINE or self.tok == Token.EOF:
                break;
            else:
                current_expression = self._expression();
            assert(current_expression != None);

            if self.eat(Token.COMMA):
                if list is None:
                    list = ast.ExpressionList(self.loc, []);
                if current_expression is not None:
                    list.values.append(current_expression);
            else:
                if list is None:
                    return current_expression;
                if current_expression is not None:
                    list.values.append(current_expression)
                return list;
        return list;
