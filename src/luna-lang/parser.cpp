module;

#include <karm-core/macros.h>

export module Luna:parser;

import Karm.Logger;
import :expr;

namespace Luna {

// MARK: Lexer -----------------------------------------------------------------

export struct Token {
    enum struct Kind {
        INVALID, // Catch-all for lexer errors
        EOF,

        COMMENT, // // /* */
        WHITESPACE,

        IDENT,   // foo, bar
        INTEGER, // 123
        NUMBER,  // 12.34

        LSTR, // "
        RSTR, // "
        SPAN, // span of text

        FN,    // fn
        VAR,   // var
        CONST, // const

        IF,    // if
        ELSE,  // else
        FOR,   // or
        WHILE, // while
        TRY,   // try
        CATCH, // catch

        RETURN,   // return
        BREAK,    // break
        CONTINUE, // continue
        THROW,    // throw

        NONE,
        TRUE,
        FALSE,

        IS,     // is
        AS,     // as
        TYPEOF, // typeof

        AND, // and
        OR,  // or
        NOT, // not

        LPAREN,   // (
        RPAREN,   // )
        LBRACKET, // [
        RBRACKET, // ]
        LBRACE,   // {
        RBRACE,   // }

        COMMA,     // ,
        HASH,      // #
        DOT,       // .
        COLON,     // :
        SEMICOLON, // ;

        ASSIGN, // =

        EQ,   // ==
        NEQ,  // !=
        LT,   // <
        LTEQ, // <=
        GT,   // >
        GTEQ, // >=

        PLUS,    // +
        MINUS,   // -
        STAR,    // *
        SLASH,   // /
        PERCENT, // %

        TILDE,     // ~
        AMPERSAND, // &
        PIPE,      // |
        CARET,     // ^

        _LEN,
    };
    using enum Kind;

    Kind kind;
    Str text;

    void repr(Io::Emit& e) const {
        e("({} {#})", kind, text);
    }

    bool operator==(Kind other) const {
        return kind == other;
    }

    bool operator==(Token const&) const = default;
};

export CompletionOr<Vec<Token>> lex(Io::SScan& s) {
    Vec<Token> tokens{};

    while (not s.ended()) {
        s.begin();

        if (s.eat(Re::space())) {
            tokens.emplaceBack(Token::WHITESPACE, s.end());
            continue;
        }

        if (s.skip("//")) {
            while (not s.ended() and s.peek() != '\n') {
                s.next();
            }
            tokens.emplaceBack(Token::COMMENT, s.end());
            continue;
        }
        if (s.skip("/*")) {
            while (not s.ended()) {
                if (s.skip("*/"))
                    break;
                s.next();
            }
            tokens.emplaceBack(Token::COMMENT, s.end());
            continue;
        }

        if (s.skip(Re::alpha())) {
            s.eat(Re::alnum() | '_'_re);

            Str text = s.end();
            Token::Kind kind = Token::IDENT;

            // Keyword Mapping
            if (text == "fn")
                kind = Token::FN;
            else if (text == "var")
                kind = Token::VAR;
            else if (text == "const")
                kind = Token::CONST;
            else if (text == "if")
                kind = Token::IF;
            else if (text == "else")
                kind = Token::ELSE;
            else if (text == "for")
                kind = Token::FOR;
            else if (text == "while")
                kind = Token::WHILE;
            else if (text == "return")
                kind = Token::RETURN;
            else if (text == "break")
                kind = Token::BREAK;
            else if (text == "continue")
                kind = Token::CONTINUE;
            else if (text == "none")
                kind = Token::NONE;
            else if (text == "true")
                kind = Token::TRUE;
            else if (text == "false")
                kind = Token::FALSE;
            else if (text == "and")
                kind = Token::AND;
            else if (text == "is")
                kind = Token::IS;
            else if (text == "as")
                kind = Token::AS;
            else if (text == "typeof")
                kind = Token::TYPEOF;
            else if (text == "or")
                kind = Token::OR;
            else if (text == "not")
                kind = Token::NOT;

            tokens.emplaceBack(kind, text);
            continue;
        }

        if (s.skip(Re::digit())) {
            s.eat(Re::digit());

            if (s.peek() == '.' and s.peek(1) >= '0' and s.peek(1) <= '9') {
                s.next(); // consume dot
                s.eat(Re::digit());
                tokens.emplaceBack(Token::NUMBER, s.end());
            } else {
                tokens.emplaceBack(Token::INTEGER, s.end());
            }
            continue;
        }

        if (s.skip('"')) {
            tokens.emplaceBack(Token::LSTR, s.end());

            s.begin();
            while (not s.ended() and s.peek() != '"') {
                if (s.peek() == '\\') {
                    s.next();
                }
                s.next();
            }
            tokens.emplaceBack(Token::SPAN, s.end());

            s.begin();
            if (s.skip('"')) {
                tokens.emplaceBack(Token::RSTR, s.end());
            } else {
                return Completion::exception("unterminated string literal");
            }
            continue;
        }

        if (s.skip("==")) {
            tokens.emplaceBack(Token::EQ, s.end());
            continue;
        }
        if (s.skip("!=")) {
            tokens.emplaceBack(Token::NEQ, s.end());
            continue;
        }
        if (s.skip("<=")) {
            tokens.emplaceBack(Token::LTEQ, s.end());
            continue;
        }
        if (s.skip(">=")) {
            tokens.emplaceBack(Token::GTEQ, s.end());
            continue;
        }

        if (s.skip('(')) {
            tokens.emplaceBack(Token::LPAREN, s.end());
            continue;
        }
        if (s.skip(')')) {
            tokens.emplaceBack(Token::RPAREN, s.end());
            continue;
        }
        if (s.skip('[')) {
            tokens.emplaceBack(Token::LBRACKET, s.end());
            continue;
        }
        if (s.skip(']')) {
            tokens.emplaceBack(Token::RBRACKET, s.end());
            continue;
        }
        if (s.skip('{')) {
            tokens.emplaceBack(Token::LBRACE, s.end());
            continue;
        }
        if (s.skip('}')) {
            tokens.emplaceBack(Token::RBRACE, s.end());
            continue;
        }

        if (s.skip(',')) {
            tokens.emplaceBack(Token::COMMA, s.end());
            continue;
        }
        if (s.skip('#')) {
            tokens.emplaceBack(Token::HASH, s.end());
            continue;
        }
        if (s.skip('.')) {
            tokens.emplaceBack(Token::DOT, s.end());
            continue;
        }
        if (s.skip(':')) {
            tokens.emplaceBack(Token::COLON, s.end());
            continue;
        }
        if (s.skip(';')) {
            tokens.emplaceBack(Token::SEMICOLON, s.end());
            continue;
        }

        if (s.skip('=')) {
            tokens.emplaceBack(Token::ASSIGN, s.end());
            continue;
        }
        if (s.skip('<')) {
            tokens.emplaceBack(Token::LT, s.end());
            continue;
        }
        if (s.skip('>')) {
            tokens.emplaceBack(Token::GT, s.end());
            continue;
        }

        if (s.skip('+')) {
            tokens.emplaceBack(Token::PLUS, s.end());
            continue;
        }
        if (s.skip('-')) {
            tokens.emplaceBack(Token::MINUS, s.end());
            continue;
        }
        if (s.skip('*')) {
            tokens.emplaceBack(Token::STAR, s.end());
            continue;
        }
        if (s.skip('/')) {
            tokens.emplaceBack(Token::SLASH, s.end());
            continue;
        }
        if (s.skip('%')) {
            tokens.emplaceBack(Token::PERCENT, s.end());
            continue;
        }

        if (s.skip('~')) {
            tokens.emplaceBack(Token::TILDE, s.end());
            continue;
        }
        if (s.skip('&')) {
            tokens.emplaceBack(Token::AMPERSAND, s.end());
            continue;
        }
        if (s.skip('|')) {
            tokens.emplaceBack(Token::PIPE, s.end());
            continue;
        }
        if (s.skip('^')) {
            tokens.emplaceBack(Token::CARET, s.end());
            continue;
        }

        s.next();
        tokens.emplaceBack(Token::INVALID, s.end());
    }

    s.begin();
    tokens.emplaceBack(Token::EOF, s.end());

    return Ok(tokens);
}

// MARK: Parser ----------------------------------------------------------------

enum struct Prec {
    LOWEST,

    ASSIGN,     // =
    OR,         // or
    AND,        // and
    EQUALITY,   // == !=
    TYPE,       // is as
    COMPARISON, // < > <= >=
    TERM,       // + -
    FACTOR,     // * / %
    UNARY,      // ! - +
    CALL,       // . () []

    HIGHEST
};

static CompletionOr<Value> _intoAssign(Value lhs, Value rhs) {
    if (isSymbol(lhs)) {
        return opNew<SetEnvExpr>(lhs, rhs);
    }

    if (isObject(lhs)) {
        auto obj = try$(asObject(lhs));
        if (auto s = obj.is<GetExpr>()) {
            return opNew<SetExpr>(s->_target, s->_key, rhs);
        }
    }

    return Completion::exception("not assignable");
}

static void _eatWhitespace(Cursor<Token>& c) {
    while (*c == Token::WHITESPACE or *c == Token::COMMENT)
        c.next();
}

static CompletionOr<Symbol> _parseIdent(Cursor<Token>& c) {
    if (*c == Token::IDENT)
        return Ok(Symbol::from(c.next().text));
    return Completion::exception("expected ident");
}

static CompletionOr<Value> _parseValue(Cursor<Token>& c) {
    if (c.skip(Token::NONE)) {
        return Ok(NONE);
    } else if (c.skip(Token::TRUE)) {
        return Ok(true);
    } else if (c.skip(Token::FALSE)) {
        return Ok(false);
    } else if (*c == Token::INTEGER) {
        return Ok(Io::atoi(c.next().text).take());
    } else if (*c == Token::NUMBER) {
        return Ok(Io::atof(c.next().text).take());
    } else if (c.skip(Token::LSTR)) {
        if (*c != Token::SPAN)
            return Completion::exception("expected text span");
        String str = c.next().text;

        if (!c.skip(Token::RSTR))
            return Completion::exception("expected closing");

        return Ok(str);
    } else {
        return Completion::exception("expected value");
    }
}

static CompletionOr<Value> _parseIdentOrValue(Cursor<Token>& c) {
    if (*c == Token::IDENT)
        return _parseIdent(c);
    return _parseValue(c);
}

static CompletionOr<Value> _parseExpr(Cursor<Token>& c, Prec prec);

static CompletionOr<Value> _parseVar(Cursor<Token>& c) {
    c.next();
    _eatWhitespace(c);

    auto ident = try$(_parseIdent(c));
    _eatWhitespace(c);

    if (not c.skip(Token::ASSIGN))
        return Completion::exception("expected '='");

    _eatWhitespace(c);
    auto expr = try$(_parseExpr(c, Prec::LOWEST));

    return opNew<DeclExpr>(ident, expr);
}

static CompletionOr<Value> _parseIf(Cursor<Token>& c) {
    c.next();
    _eatWhitespace(c);

    auto cond = try$(_parseExpr(c, Prec::LOWEST));
    _eatWhitespace(c);

    auto then = try$(_parseExpr(c, Prec::LOWEST));

    _eatWhitespace(c);
    if (c.skip(Token::ELSE)) {

        _eatWhitespace(c);
        auto else_ = try$(_parseExpr(c, Prec::LOWEST));

        return opNew<IfExpr>(cond, then, else_);
    }

    return opNew<IfExpr>(cond, then, NONE);
}

static CompletionOr<Value> _parseWhile(Cursor<Token>& c) {
    c.next();
    _eatWhitespace(c);

    auto cond = try$(_parseExpr(c, Prec::LOWEST));
    _eatWhitespace(c);

    auto body = try$(_parseExpr(c, Prec::LOWEST));

    return opNew<WhileExpr>(cond, body);
}

static CompletionOr<Value> _parseTry(Cursor<Token>& c) {
    c.next();
    _eatWhitespace(c);

    auto try_ = try$(_parseExpr(c, Prec::LOWEST));
    _eatWhitespace(c);

    if (not c.skip(Token::CATCH))
        return Completion::exception("expected 'catch'");

    _eatWhitespace(c);
    if (not c.skip(Token::LPAREN))
        return Completion::exception("expected '('");

    _eatWhitespace(c);
    auto ident = try$(_parseIdent(c));
    _eatWhitespace(c);

    if (not c.skip(Token::RPAREN))
        return Completion::exception("expected ')'");

    _eatWhitespace(c);
    auto catch_ = try$(_parseExpr(c, Prec::LOWEST));

    return opNew<TryExpr>(try_, ident, catch_);
}

static CompletionOr<Value> _parseFunc(Cursor<Token>& c) {
    c.next();
    _eatWhitespace(c);

    if (not c.skip(Token::LPAREN))
        return Completion::exception("expected '('");

    Vec<ParamExpr> sig;
    _eatWhitespace(c);
    if (not c.skip(Token::RPAREN)) {
        do {
            _eatWhitespace(c);
            auto key = try$(_parseIdent(c));
            _eatWhitespace(c);

            Opt<Value> value;
            if (c.skip(Token::COLON)) {
                _eatWhitespace(c);
                value = try$(_parseExpr(c, Prec::LOWEST));
                _eatWhitespace(c);
            }

            sig.pushBack({key, value});
        } while (c.skip(Token::COMMA));

        if (not c.skip(Token::RPAREN))
            return Completion::exception("expected ')'");
    }

    _eatWhitespace(c);
    auto code = try$(_parseExpr(c, Prec::LOWEST));

    return opNew<FuncExpr>(sig, code);
}

static CompletionOr<Value> _parseParent(Cursor<Token>& c) {
    c.next();
    _eatWhitespace(c);
    auto expr = try$(_parseExpr(c, Prec::LOWEST));
    _eatWhitespace(c);
    if (not c.skip(Token::RPAREN))
        return Completion::exception("expected ')'");
    return Ok(expr);
}

static bool _isTableHaead(Cursor<Token> c) {
    if (not c.skip(Token::LBRACE))
        return false;

    _eatWhitespace(c);
    if (c.skip(Token::RBRACE))
        return true;

    _eatWhitespace(c);
    if (not _parseIdentOrValue(c))
        return false;

    _eatWhitespace(c);
    if (not c.skip(Token::COLON))
        return false;

    return true;
}

static CompletionOr<Value> _parseTable(Cursor<Token>& c) {
    c.next();
    _eatWhitespace(c);

    if (c.skip(Token::RBRACE))
        return opNew<TableExpr>();

    Vec<Tuple<Value, Value>> exprs;
    do {
        _eatWhitespace(c);
        auto key = try$(_parseIdentOrValue(c));

        _eatWhitespace(c);
        if (not c.skip(Token::COLON))
            return Completion::exception("expected ':'");

        _eatWhitespace(c);
        auto value = try$(_parseExpr(c, Prec::LOWEST));
        exprs.pushBack({key, value});

        _eatWhitespace(c);
    } while (c.skip(Token::COMMA));

    if (c.skip(Token::RBRACE))
        return opNew<TableExpr>(exprs);

    return Completion::exception("expected '}'");
}

static CompletionOr<Value> _parseBlock(Cursor<Token>& c) {
    c.next();
    _eatWhitespace(c);

    if (c.skip(Token::RBRACE))
        return opNew<BlockExpr>();

    Vec<Value> exprs;
    do {
        _eatWhitespace(c);
        if (*c == Token::RBRACE)
            break;
        _eatWhitespace(c);
        exprs.pushBack(try$(_parseExpr(c, Prec::LOWEST)));

        _eatWhitespace(c);
    } while (c.skip(Token::SEMICOLON));

    _eatWhitespace(c);
    if (c.skip(Token::RBRACE))
        return opNew<BlockExpr>(exprs);

    return Completion::exception("expected '}'");
}

static CompletionOr<Value> _parseList(Cursor<Token>& c) {
    c.next();
    _eatWhitespace(c);

    if (c.skip(Token::RBRACKET))
        return opNew<ListExpr>();

    Vec<Value> exprs;
    do {
        _eatWhitespace(c);
        exprs.pushBack(try$(_parseExpr(c, Prec::LOWEST)));
        _eatWhitespace(c);
    } while (c.skip(Token::COMMA));

    if (c.skip(Token::RBRACKET))
        return opNew<ListExpr>(exprs);

    return Completion::exception("expected ']'");
}

static CompletionOr<Value> _parsePrefix(Cursor<Token>& c) {
    auto t = c.peek();
    switch (t.kind) {
    case Token::LPAREN:
        return _parseParent(c);

    case Token::LBRACE: {
        if (_isTableHaead(c))
            return _parseTable(c);
        return _parseBlock(c);
    }

    case Token::LBRACKET:
        return _parseList(c);

    case Token::NOT: {
        c.next();
        _eatWhitespace(c);
        return opNew<NotExpr>(try$(_parseExpr(c, Prec::UNARY)));
    }
    case Token::MINUS: {
        c.next();
        _eatWhitespace(c);
        return opNew<NegExpr>(try$(_parseExpr(c, Prec::UNARY)));
    }
    case Token::HASH: {
        c.next();
        _eatWhitespace(c);
        return opNew<QuoteExpr>(try$(_parseExpr(c, Prec::UNARY)));
    }

    case Token::IDENT:
        return _parseIdent(c);

    case Token::VAR:
        return _parseVar(c);

    case Token::RETURN: {
        c.next();
        _eatWhitespace(c);
        if (*c == Token::SEMICOLON)
            return opNew<ReturnExpr>(NONE);
        return opNew<ReturnExpr>(try$(_parseExpr(c, Prec::LOWEST)));
    }

    case Token::CONTINUE: {
        c.next();
        _eatWhitespace(c);
        if (*c == Token::SEMICOLON)
            return opNew<ContinueExpr>(NONE);
        return opNew<ContinueExpr>(try$(_parseExpr(c, Prec::LOWEST)));
    }

    case Token::BREAK: {
        c.next();
        _eatWhitespace(c);
        if (*c == Token::SEMICOLON)
            return opNew<BreakExpr>(NONE);
        return opNew<BreakExpr>(try$(_parseExpr(c, Prec::LOWEST)));
    }

    case Token::IF:
        return _parseIf(c);

    case Token::WHILE:
        return _parseWhile(c);

    case Token::TRY:
        return _parseTry(c);

    case Token::FN:
        return _parseFunc(c);

    case Token::TYPEOF: {
        c.next();
        _eatWhitespace(c);
        return opNew<TypeOfExpr>(try$(_parseExpr(c, Prec::UNARY)));
    }

    default:
        return _parseValue(c);
    }
}

static Prec _peekPrec(Cursor<Token>& c) {
    auto t = c.peek();
    switch (t.kind) {
    case Token::ASSIGN:
        return Prec::ASSIGN;
    case Token::OR:
        return Prec::OR;
    case Token::AND:
        return Prec::AND;
    case Token::EQ:
    case Token::NEQ:
        return Prec::EQUALITY;
    case Token::LT:
    case Token::LTEQ:
    case Token::GT:
    case Token::GTEQ:
        return Prec::COMPARISON;
    case Token::PLUS:
    case Token::MINUS:
        return Prec::TERM;
    case Token::STAR:
    case Token::SLASH:
    case Token::PERCENT:
        return Prec::FACTOR;
    case Token::LPAREN:
    case Token::DOT:
    case Token::LBRACKET:
    case Token::IS:
    case Token::AS:
        return Prec::CALL;
    default:
        return Prec::LOWEST;
    }
}

static CompletionOr<Value> _parseCall(Cursor<Token>& c, Value func) {
    c.next();
    _eatWhitespace(c);

    Vec<ArgExpr> args;
    if (not c.skip(Token::RPAREN)) {
        do {
            _eatWhitespace(c);
            Opt<Value> key;
            if (*c == Token::IDENT) {
                auto next = c;
                next.next();
                _eatWhitespace(next);
                if (next.skip(Token::COLON)) {
                    key = try$(_parseIdent(c));
                    _eatWhitespace(c);
                    c.next(); // skip colon
                    _eatWhitespace(c);
                }
            }

            auto expr = try$(_parseExpr(c, Prec::LOWEST));
            args.pushBack(ArgExpr{key, expr});
            _eatWhitespace(c);
        } while (c.skip(Token::COMMA));

        if (not c.skip(Token::RPAREN))
            return Completion::exception("expected ')'");
    }

    return opNew<CallExpr>(func, args);
}

static CompletionOr<Value> _parseIndex(Cursor<Token>& c, Value lhs) {
    auto rhs = try$(_parseExpr(c, Prec::LOWEST));
    _eatWhitespace(c);
    if (not c.skip(Token::RBRACKET))
        return Completion::exception("expected ']'");
    return opNew<GetExpr>(lhs, rhs);
}

static CompletionOr<Value> _parseInfix(Cursor<Token>& c, Value lhs) {
    Token op = c.next();
    _eatWhitespace(c);
    switch (op.kind) {
    case Token::ASSIGN:
        return _intoAssign(lhs, try$(_parseExpr(c, Prec::ASSIGN)));

    case Token::OR:
        return opNew<OrExpr>(lhs, try$(_parseExpr(c, Prec::OR)));
    case Token::AND:
        return opNew<AndExpr>(lhs, try$(_parseExpr(c, Prec::AND)));

    case Token::EQ:
        return opNew<EqExpr>(lhs, try$(_parseExpr(c, Prec::EQUALITY)));
    case Token::NEQ:
        return opNew<NEqExpr>(lhs, try$(_parseExpr(c, Prec::EQUALITY)));

    case Token::LT:
        return opNew<LtExpr>(lhs, try$(_parseExpr(c, Prec::COMPARISON)));
    case Token::LTEQ:
        return opNew<LtEqExpr>(lhs, try$(_parseExpr(c, Prec::COMPARISON)));
    case Token::GT:
        return opNew<GtExpr>(lhs, try$(_parseExpr(c, Prec::COMPARISON)));
    case Token::GTEQ:
        return opNew<GtEqExpr>(lhs, try$(_parseExpr(c, Prec::COMPARISON)));

    case Token::PLUS:
        return opNew<AddExpr>(lhs, try$(_parseExpr(c, Prec::TERM)));
    case Token::MINUS:
        return opNew<SubExpr>(lhs, try$(_parseExpr(c, Prec::TERM)));
    case Token::STAR:
        return opNew<MulExpr>(lhs, try$(_parseExpr(c, Prec::FACTOR)));
    case Token::SLASH:
        return opNew<DivExpr>(lhs, try$(_parseExpr(c, Prec::FACTOR)));
    case Token::PERCENT:
        return opNew<ModExpr>(lhs, try$(_parseExpr(c, Prec::FACTOR)));

    case Token::DOT:
        return opNew<GetExpr>(lhs, try$(_parseExpr(c, Prec::CALL)));
    case Token::LPAREN:
        return _parseCall(c, lhs);
    case Token::LBRACKET:
        return _parseIndex(c, lhs);
    case Token::IS:
        return opNew<IsExpr>(lhs, try$(_parseExpr(c, Prec::TYPE)));
    case Token::AS:
        return opNew<AsExpr>(lhs, try$(_parseExpr(c, Prec::TYPE)));

    default:
        return Completion::exception("unexpected infix operator");
    }
}

static CompletionOr<Value> _parseExpr(Cursor<Token>& c, Prec minPrec) {
    auto lhs = try$(_parsePrefix(c));
    while (not c.ended()) {
        _eatWhitespace(c);

        Prec nextPrec = _peekPrec(c);
        if (nextPrec <= minPrec)
            break;
        lhs = try$(_parseInfix(c, lhs));
    }
    return Ok(lhs);
}

static CompletionOr<Value> _parseTopLevel(Cursor<Token>& c) {
    Vec<Value> exprs;
    do {
        _eatWhitespace(c);
        if (c.skip(Token::EOF))
            break;
        exprs.pushBack(try$(_parseExpr(c, Prec::LOWEST)));
        _eatWhitespace(c);
    } while (c.skip(Token::SEMICOLON) or *c != Token::EOF);

    return opNew<BlockExpr>(exprs);
}

export CompletionOr<Value> parse(Str code) {
    Io::SScan s{code};
    auto tokens = try$(lex(s));
    Cursor<Token> c{tokens};
    _eatWhitespace(c);
    return _parseTopLevel(c);
}

} // namespace Luna
