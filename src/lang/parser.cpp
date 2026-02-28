module;

#include <karm/macros>

export module Luna:parser;

import Karm.Diag;
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

        IF,     // if
        ELSE,   // else
        FOR,    // or
        WHILE,  // while
        TRY,    // try
        CATCH,  // catch
        ASSERT, // assert

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
    Io::LocSpan span;

    void repr(Io::Emit& e) const {
        e("({} {#} at {})", kind, text, span);
    }

    bool operator==(Kind other) const {
        return kind == other;
    }

    bool operator==(Token const&) const = default;

    static Str kindName(Kind k) {
        switch (k) {
        case INVALID:
            return "invalid token"s;
        case EOF:
            return "end of file"s;
        case COMMENT:
            return "comment"s;
        case WHITESPACE:
            return "whitespace"s;
        case IDENT:
            return "identifier"s;
        case INTEGER:
            return "integer"s;
        case NUMBER:
            return "number"s;
        case LSTR:
            return "string start"s;
        case RSTR:
            return "string end"s;
        case SPAN:
            return "text span"s;
        case FN:
            return "'fn'"s;
        case VAR:
            return "'var'"s;
        case CONST:
            return "'const'"s;
        case IF:
            return "'if'"s;
        case ELSE:
            return "'else'"s;
        case FOR:
            return "'for'"s;
        case WHILE:
            return "'while'"s;
        case TRY:
            return "'try'"s;
        case CATCH:
            return "'catch'"s;
        case ASSERT:
            return "'assert'"s;
        case RETURN:
            return "'return'"s;
        case BREAK:
            return "'break'"s;
        case CONTINUE:
            return "'continue'"s;
        case THROW:
            return "'throw'"s;
        case NONE:
            return "'none'"s;
        case TRUE:
            return "'true'"s;
        case FALSE:
            return "'false'"s;
        case IS:
            return "'is'"s;
        case AS:
            return "'as'"s;
        case TYPEOF:
            return "'typeof'"s;
        case AND:
            return "'and'"s;
        case OR:
            return "'or'"s;
        case NOT:
            return "'not'"s;
        case LPAREN:
            return "'('"s;
        case RPAREN:
            return "')'"s;
        case LBRACKET:
            return "'['"s;
        case RBRACKET:
            return "']'"s;
        case LBRACE:
            return "'{'"s;
        case RBRACE:
            return "'}'"s;
        case COMMA:
            return "','"s;
        case HASH:
            return "'#'"s;
        case DOT:
            return "'.'"s;
        case COLON:
            return "':'"s;
        case SEMICOLON:
            return "';'"s;
        case ASSIGN:
            return "'='"s;
        case EQ:
            return "'=='"s;
        case NEQ:
            return "'!='"s;
        case LT:
            return "'<'"s;
        case LTEQ:
            return "'<='"s;
        case GT:
            return "'>'"s;
        case GTEQ:
            return "'>='"s;
        case PLUS:
            return "'+'"s;
        case MINUS:
            return "'-'"s;
        case STAR:
            return "'*'"s;
        case SLASH:
            return "'/'"s;
        case PERCENT:
            return "'%'"s;
        case TILDE:
            return "'~'"s;
        case AMPERSAND:
            return "'&'"s;
        case PIPE:
            return "'|'"s;
        case CARET:
            return "'^'"s;
        default:
            return "unknown"s;
        }
    }
};

// MARK: Diagnostics ----------------------------------------------------------

export struct DiagCollector {
    Str source;
    Vec<Diag::Diagnostic> diags;

    DiagCollector(Str src) : source(src) {}

    void emit(Diag::Diagnostic d) {
        diags.pushBack(std::move(d));
    }

    Completion fatal(Diag::Diagnostic d) {
        emit(std::move(d));
        return Completion::exception("parse error"s);
    }

    Completion expected(Str expected, Token const& got) {
        return fatal(
            Diag::Diagnostic::error("E0100", Io::format("expected {}, found {}", expected, Token::kindName(got.kind)))
                .withPrimaryLabel(got.span, Io::format("expected {} here", expected))
        );
    }

    Completion unexpected(Token const& tok, Str context = ""s) {
        String msg = context.len() > 0
                         ? Io::format("unexpected {} in {}", Token::kindName(tok.kind), context)
                         : Io::format("unexpected {}", Token::kindName(tok.kind));
        return fatal(Diag::Diagnostic::error("E0101", msg).withPrimaryLabel(tok.span, "unexpected token"s));
    }

    String format() const {
        Io::StringWriter w;
        Diag::Renderer renderer{source};
        for (auto const& d : diags)
            renderer.render(w, d);
        return w.take();
    }

    void dumpTo(Io::TextWriter& w) const {
        Diag::Renderer renderer{source};
        for (auto const& d : diags)
            renderer.render(w, d);
    }
};

// Keyword lookup table
static Map<Str, Token::Kind> const KEYWORDS = {
    {"fn"s, Token::FN},
    {"var"s, Token::VAR},
    {"const"s, Token::CONST},
    {"if"s, Token::IF},
    {"else"s, Token::ELSE},
    {"for"s, Token::FOR},
    {"while"s, Token::WHILE},
    {"try"s, Token::TRY},
    {"catch"s, Token::CATCH},
    {"assert"s, Token::ASSERT},
    {"return"s, Token::RETURN},
    {"break"s, Token::BREAK},
    {"continue"s, Token::CONTINUE},
    {"throw"s, Token::THROW},
    {"none"s, Token::NONE},
    {"true"s, Token::TRUE},
    {"false"s, Token::FALSE},
    {"and"s, Token::AND},
    {"or"s, Token::OR},
    {"not"s, Token::NOT},
    {"is"s, Token::IS},
    {"as"s, Token::AS},
    {"typeof"s, Token::TYPEOF},
};

export CompletionOr<Vec<Token>> lex(Io::SScan& s, DiagCollector& diag) {
    Vec<Token> tokens{};

    while (not s.ended()) {
        s.begin();
        Io::Loc startLoc = s.loc();

        // Skip whitespace (don't emit tokens)
        if (s.eat(Re::space()))
            continue;

        // Skip comments (don't emit tokens)
        if (s.skip("//")) {
            while (not s.ended() and s.peek() != '\n')
                s.next();
            continue;
        }
        if (s.skip("/*")) {
            while (not s.ended()) {
                if (s.skip("*/"))
                    break;
                s.next();
            }
            continue;
        }

        if (s.skip(Re::alpha())) {
            s.eat(Re::alnum() | '_'_re);

            Str text = s.end();
            Io::Loc endLoc = s.loc();
            Token::Kind kind = KEYWORDS.lookup(text).unwrapOr(Token::IDENT);

            tokens.emplaceBack(kind, text, Io::LocSpan{startLoc, endLoc});
            continue;
        }

        if (s.skip(Re::digit())) {
            s.eat(Re::digit());

            if (s.peek() == '.' and s.peek(1) >= '0' and s.peek(1) <= '9') {
                s.next(); // consume dot
                s.eat(Re::digit());
                Io::Loc endLoc = s.loc();
                tokens.emplaceBack(Token::NUMBER, s.end(), Io::LocSpan{startLoc, endLoc});
            } else {
                Io::Loc endLoc = s.loc();
                tokens.emplaceBack(Token::INTEGER, s.end(), Io::LocSpan{startLoc, endLoc});
            }
            continue;
        }

        if (s.skip('"')) {
            Io::Loc endLoc = s.loc();
            tokens.emplaceBack(Token::LSTR, s.end(), Io::LocSpan{startLoc, endLoc});

            s.begin();
            Io::Loc spanStart = s.loc();
            while (not s.ended() and s.peek() != '"') {
                if (s.peek() == '\\')
                    s.next();
                s.next();
            }
            Io::Loc spanEnd = s.loc();
            tokens.emplaceBack(Token::SPAN, s.end(), Io::LocSpan{spanStart, spanEnd});

            s.begin();
            Io::Loc closeStart = s.loc();
            if (s.skip('"')) {
                Io::Loc closeEnd = s.loc();
                tokens.emplaceBack(Token::RSTR, s.end(), Io::LocSpan{closeStart, closeEnd});
            } else {
                return diag.fatal(Diag::Diagnostic::error("E0001", "unterminated string literal").withPrimaryLabel(Io::LocSpan{startLoc, spanEnd}, "string started here").withHelp("add a closing '\"' to terminate the string"));
            }
            continue;
        }

        // Two-character operators
        if (s.skip("==")) {
            Io::Loc endLoc = s.loc();
            tokens.emplaceBack(Token::EQ, s.end(), Io::LocSpan{startLoc, endLoc});
            continue;
        }
        if (s.skip("!=")) {
            Io::Loc endLoc = s.loc();
            tokens.emplaceBack(Token::NEQ, s.end(), Io::LocSpan{startLoc, endLoc});
            continue;
        }
        if (s.skip("<=")) {
            Io::Loc endLoc = s.loc();
            tokens.emplaceBack(Token::LTEQ, s.end(), Io::LocSpan{startLoc, endLoc});
            continue;
        }
        if (s.skip(">=")) {
            Io::Loc endLoc = s.loc();
            tokens.emplaceBack(Token::GTEQ, s.end(), Io::LocSpan{startLoc, endLoc});
            continue;
        }

// Single-character tokens
#define SINGLE_CHAR_TOKEN(ch, kind)                                              \
    if (s.skip(ch)) {                                                            \
        Io::Loc endLoc = s.loc();                                                \
        tokens.emplaceBack(Token::kind, s.end(), Io::LocSpan{startLoc, endLoc}); \
        continue;                                                                \
    }

        SINGLE_CHAR_TOKEN('(', LPAREN)
        SINGLE_CHAR_TOKEN(')', RPAREN)
        SINGLE_CHAR_TOKEN('[', LBRACKET)
        SINGLE_CHAR_TOKEN(']', RBRACKET)
        SINGLE_CHAR_TOKEN('{', LBRACE)
        SINGLE_CHAR_TOKEN('}', RBRACE)
        SINGLE_CHAR_TOKEN(',', COMMA)
        SINGLE_CHAR_TOKEN('#', HASH)
        SINGLE_CHAR_TOKEN('.', DOT)
        SINGLE_CHAR_TOKEN(':', COLON)
        SINGLE_CHAR_TOKEN(';', SEMICOLON)
        SINGLE_CHAR_TOKEN('=', ASSIGN)
        SINGLE_CHAR_TOKEN('<', LT)
        SINGLE_CHAR_TOKEN('>', GT)
        SINGLE_CHAR_TOKEN('+', PLUS)
        SINGLE_CHAR_TOKEN('-', MINUS)
        SINGLE_CHAR_TOKEN('*', STAR)
        SINGLE_CHAR_TOKEN('/', SLASH)
        SINGLE_CHAR_TOKEN('%', PERCENT)
        SINGLE_CHAR_TOKEN('~', TILDE)
        SINGLE_CHAR_TOKEN('&', AMPERSAND)
        SINGLE_CHAR_TOKEN('|', PIPE)
        SINGLE_CHAR_TOKEN('^', CARET)

#undef SINGLE_CHAR_TOKEN

        // Invalid character
        s.next();
        Io::Loc endLoc = s.loc();
        return diag.fatal(
            Diag::Diagnostic::error("E0002", "unexpected character")
                .withPrimaryLabel(Io::LocSpan{startLoc, endLoc}, "unexpected character")
        );
    }

    Io::Loc eofLoc = s.loc();
    s.begin();
    tokens.emplaceBack(Token::EOF, s.end(), Io::LocSpan{eofLoc, eofLoc});

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

static CompletionOr<Value> _intoAssign(DiagCollector& diag, Value lhs, Value rhs, Io::LocSpan span) {
    if (isSymbol(lhs)) {
        return opNew<SetEnvExpr>(try$(opNew<QuoteExpr>(lhs)), rhs);
    }

    if (isObject(lhs)) {
        auto obj = try$(asObject(lhs));
        if (auto s = obj.is<GetExpr>()) {
            return opNew<SetExpr>(s->_target, s->_key, rhs);
        }
    }

    return diag.fatal(
        Diag::Diagnostic::error("E0200", "expression is not assignable")
            .withPrimaryLabel(span, "cannot assign to this expression")
            .withNote("only variables and object properties can be assigned to")
    );
}

static CompletionOr<Symbol> _parseIdent(Cursor<Token>& c, DiagCollector& diag) {
    if (*c == Token::IDENT)
        return Ok(Symbol::from(c.next().text));
    return diag.expected("identifier"s, *c);
}

static CompletionOr<Value> _parseValue(Cursor<Token>& c, DiagCollector& diag) {
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
            return diag.expected("string content"s, *c);
        String str = c.next().text;

        if (not c.skip(Token::RSTR)) {
            return diag.fatal(
                Diag::Diagnostic::error("E0102", "unterminated string literal")
                    .withPrimaryLabel(c->span, "expected closing '\"'")
                    .withHelp("add a closing '\"' to terminate the string")
            );
        }

        return Ok(str);
    } else {
        return diag.expected("value"s, *c);
    }
}

static CompletionOr<Value> _parseIdentOrValue(Cursor<Token>& c, DiagCollector& diag) {
    if (*c == Token::IDENT)
        return _parseIdent(c, diag);
    return _parseValue(c, diag);
}

static CompletionOr<Value> _parseExpr(Cursor<Token>& c, DiagCollector& diag, Prec prec);

static CompletionOr<Value> _parseVar(Cursor<Token>& c, DiagCollector& diag) {
    auto varToken = c.next(); // consume 'var'

    auto ident = try$(_parseIdent(c, diag));

    if (not c.skip(Token::ASSIGN)) {
        return diag.fatal(
            Diag::Diagnostic::error("E0103", "expected '=' in variable declaration")
                .withPrimaryLabel(c->span, "expected '=' here")
                .withSecondaryLabel(varToken.span, "variable declared here")
                .withHelp("add '=' followed by an initial value")
        );
    }
    auto expr = try$(_parseExpr(c, diag, Prec::LOWEST));

    return opNew<DeclExpr>(ident, expr);
}

static CompletionOr<Value> _parseIf(Cursor<Token>& c, DiagCollector& diag) {
    c.next(); // consume 'if'

    auto cond = try$(_parseExpr(c, diag, Prec::LOWEST));

    auto then = try$(_parseExpr(c, diag, Prec::LOWEST));
    if (c.skip(Token::ELSE)) {
        auto else_ = try$(_parseExpr(c, diag, Prec::LOWEST));
        return opNew<IfExpr>(cond, then, else_);
    }

    return opNew<IfExpr>(cond, then, NONE);
}

static CompletionOr<Value> _parseWhile(Cursor<Token>& c, DiagCollector& diag) {
    c.next(); // consume 'while'

    auto cond = try$(_parseExpr(c, diag, Prec::LOWEST));
    auto body = try$(_parseExpr(c, diag, Prec::LOWEST));

    return opNew<WhileExpr>(cond, body);
}

static CompletionOr<Value> _parseTry(Cursor<Token>& c, DiagCollector& diag) {
    auto tryToken = c.next(); // consume 'try'

    auto try_ = try$(_parseExpr(c, diag, Prec::LOWEST));

    if (not c.skip(Token::CATCH)) {
        return diag.fatal(
            Diag::Diagnostic::error("E0104", "expected 'catch' after try block")
                .withPrimaryLabel(c->span, "expected 'catch' here")
                .withSecondaryLabel(tryToken.span, "try block started here")
                .withHelp("add a catch block: catch(e) { ... }")
        );
    }
    if (not c.skip(Token::LPAREN)) {
        return diag.expected("'('"s, *c);
    }
    auto ident = try$(_parseIdent(c, diag));

    if (not c.skip(Token::RPAREN)) {
        return diag.expected("')'"s, *c);
    }
    auto catch_ = try$(_parseExpr(c, diag, Prec::LOWEST));

    return opNew<TryExpr>(try_, ident, catch_);
}

static CompletionOr<Value> _parseAssert(Cursor<Token>& c, DiagCollector& diag) {
    c.next(); // consume 'assert'
    auto expr = try$(_parseExpr(c, diag, Prec::LOWEST));
    return opNew<AssertExpr>(expr);
}

static CompletionOr<Value> _parseFunc(Cursor<Token>& c, DiagCollector& diag) {
    auto fnToken = c.next(); // consume 'fn'

    if (not c.skip(Token::LPAREN)) {
        return diag.fatal(
            Diag::Diagnostic::error("E0105", "expected '(' after 'fn'")
                .withPrimaryLabel(c->span, "expected '(' here")
                .withSecondaryLabel(fnToken.span, "function keyword here")
                .withHelp("function syntax: fn(param1, param2) { body }")
        );
    }

    Vec<ParamExpr> sig;
    if (not c.skip(Token::RPAREN)) {
        do {
            auto key = try$(_parseIdent(c, diag));

            Opt<Value> value;
            if (c.skip(Token::COLON)) {
                value = try$(_parseExpr(c, diag, Prec::LOWEST));
            }

            sig.pushBack({key, value});
        } while (c.skip(Token::COMMA));

        if (not c.skip(Token::RPAREN)) {
            return diag.expected("')'"s, *c);
        }
    }
    auto code = try$(_parseExpr(c, diag, Prec::LOWEST));

    return opNew<FuncExpr>(sig, code);
}

static CompletionOr<Value> _parseParent(Cursor<Token>& c, DiagCollector& diag) {
    auto openParen = c.next(); // consume '('
    auto expr = try$(_parseExpr(c, diag, Prec::LOWEST));
    if (not c.skip(Token::RPAREN)) {
        return diag.fatal(
            Diag::Diagnostic::error("E0106", "unclosed parenthesis")
                .withPrimaryLabel(c->span, "expected ')' here")
                .withSecondaryLabel(openParen.span, "opening '(' here")
        );
    }
    return Ok(expr);
}

static bool _isTableHead(Cursor<Token> c) {
    if (not c.skip(Token::LBRACE))
        return false;
    if (c.skip(Token::RBRACE))
        return true;
    // Check for identifier or value
    if (c.peek() != Token::IDENT and c.peek() != Token::INTEGER and
        c.peek() != Token::NUMBER and c.peek() != Token::LSTR)
        return false;
    c.next();
    if (not c.skip(Token::COLON))
        return false;

    return true;
}

static CompletionOr<Value> _parseTable(Cursor<Token>& c, DiagCollector& diag) {
    auto openBrace = c.next(); // consume '{'

    if (c.skip(Token::RBRACE))
        return opNew<TableExpr>();

    Vec<Tuple<Value, Value>> exprs;
    do {
        auto key = try$(_parseIdentOrValue(c, diag));
        if (not c.skip(Token::COLON)) {
            return diag.fatal(
                Diag::Diagnostic::error("E0107", "expected ':' in table entry")
                    .withPrimaryLabel(c->span, "expected ':' here")
                    .withHelp("table syntax: { key: value, ... }")
            );
        }
        auto value = try$(_parseExpr(c, diag, Prec::LOWEST));
        exprs.pushBack({key, value});
    } while (c.skip(Token::COMMA));

    if (c.skip(Token::RBRACE))
        return opNew<TableExpr>(exprs);

    return diag.fatal(
        Diag::Diagnostic::error("E0108", "unclosed table")
            .withPrimaryLabel(c->span, "expected '}' here")
            .withSecondaryLabel(openBrace.span, "table started here")
    );
}

static CompletionOr<Value> _parseBlock(Cursor<Token>& c, DiagCollector& diag) {
    auto openBrace = c.next(); // consume '{'

    if (c.skip(Token::RBRACE))
        return opNew<BlockExpr>();

    Vec<Value> exprs;
    do {
        if (*c == Token::RBRACE)
            break;

        exprs.pushBack(try$(_parseExpr(c, diag, Prec::LOWEST)));
    } while (c.skip(Token::SEMICOLON));

    if (c.skip(Token::RBRACE))
        return opNew<BlockExpr>(exprs);

    return diag.fatal(
        Diag::Diagnostic::error("E0109", "unclosed block")
            .withPrimaryLabel(c->span, "expected '}' or ';' here")
            .withSecondaryLabel(openBrace.span, "block started here")
            .withHelp("separate statements with ';' and close blocks with '}'")
    );
}

static CompletionOr<Value> _parseList(Cursor<Token>& c, DiagCollector& diag) {
    auto openBracket = c.next(); // consume '['

    if (c.skip(Token::RBRACKET))
        return opNew<ListExpr>();

    Vec<Value> exprs;
    do {
        exprs.pushBack(try$(_parseExpr(c, diag, Prec::LOWEST)));
    } while (c.skip(Token::COMMA));

    if (c.skip(Token::RBRACKET))
        return opNew<ListExpr>(exprs);

    return diag.fatal(
        Diag::Diagnostic::error("E0110", "unclosed list")
            .withPrimaryLabel(c->span, "expected ']' or ',' here")
            .withSecondaryLabel(openBracket.span, "list started here")
    );
}

static CompletionOr<Value> _parsePrefix(Cursor<Token>& c, DiagCollector& diag) {
    auto t = *c;
    switch (t.kind) {
    case Token::LPAREN:
        return _parseParent(c, diag);

    case Token::LBRACE: {
        if (_isTableHead(c))
            return _parseTable(c, diag);
        return _parseBlock(c, diag);
    }

    case Token::LBRACKET:
        return _parseList(c, diag);

    case Token::NOT: {
        c.next();
        return opNew<NotExpr>(try$(_parseExpr(c, diag, Prec::UNARY)));
    }
    case Token::MINUS: {
        c.next();
        return opNew<NegExpr>(try$(_parseExpr(c, diag, Prec::UNARY)));
    }
    case Token::HASH: {
        c.next();
        return opNew<QuoteExpr>(try$(_parseExpr(c, diag, Prec::UNARY)));
    }

    case Token::IDENT:
        return _parseIdent(c, diag);

    case Token::VAR:
        return _parseVar(c, diag);

    case Token::RETURN: {
        c.next();
        if (*c == Token::SEMICOLON)
            return opNew<ReturnExpr>(NONE);
        return opNew<ReturnExpr>(try$(_parseExpr(c, diag, Prec::LOWEST)));
    }

    case Token::CONTINUE: {
        c.next();
        if (*c == Token::SEMICOLON)
            return opNew<ContinueExpr>(NONE);
        return opNew<ContinueExpr>(try$(_parseExpr(c, diag, Prec::LOWEST)));
    }

    case Token::BREAK: {
        c.next();
        if (*c == Token::SEMICOLON)
            return opNew<BreakExpr>(NONE);
        return opNew<BreakExpr>(try$(_parseExpr(c, diag, Prec::LOWEST)));
    }

    case Token::THROW: {
        c.next();
        if (*c == Token::SEMICOLON)
            return opNew<ThrowExpr>(NONE);
        return opNew<ThrowExpr>(try$(_parseExpr(c, diag, Prec::LOWEST)));
    }

    case Token::IF:
        return _parseIf(c, diag);

    case Token::WHILE:
        return _parseWhile(c, diag);

    case Token::TRY:
        return _parseTry(c, diag);

    case Token::ASSERT:
        return _parseAssert(c, diag);

    case Token::FN:
        return _parseFunc(c, diag);

    case Token::TYPEOF: {
        c.next();
        return opNew<TypeOfExpr>(try$(_parseExpr(c, diag, Prec::UNARY)));
    }

    default:
        return _parseValue(c, diag);
    }
}

static Prec _peekPrec(Cursor<Token>& c) {
    auto t = *c;
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

static CompletionOr<Value> _parseCall(Cursor<Token>& c, DiagCollector& diag, Value func, Token openParen) {
    // Note: The LPAREN token is already consumed by _parseInfix

    Vec<ArgExpr> args;
    if (not c.skip(Token::RPAREN)) {
        do {
            Opt<Value> key;
            if (*c == Token::IDENT) {
                auto next = c;
                next.next();

                if (next.skip(Token::COLON)) {
                    key = try$(_parseIdent(c, diag));
                    c.next(); // skip colon
                }
            }

            auto expr = try$(_parseExpr(c, diag, Prec::LOWEST));
            args.pushBack(ArgExpr{key, expr});
        } while (c.skip(Token::COMMA));

        if (not c.skip(Token::RPAREN)) {
            return diag.fatal(
                Diag::Diagnostic::error("E0111", "unclosed function call")
                    .withPrimaryLabel(c->span, "expected ')' here")
                    .withSecondaryLabel(openParen.span, "opening '(' here")
            );
        }
    }

    return opNew<CallExpr>(func, args);
}

static CompletionOr<Value> _parseIndex(Cursor<Token>& c, DiagCollector& diag, Value lhs, Token openBracket) {
    auto rhs = try$(_parseExpr(c, diag, Prec::LOWEST));
    if (not c.skip(Token::RBRACKET)) {
        return diag.fatal(
            Diag::Diagnostic::error("E0112", "unclosed index expression")
                .withPrimaryLabel(c->span, "expected ']' here")
                .withSecondaryLabel(openBracket.span, "opening '[' here")
        );
    }
    return opNew<GetExpr>(lhs, rhs);
}

static CompletionOr<Value> _parseInfix(Cursor<Token>& c, DiagCollector& diag, Value lhs, Io::LocSpan lhsSpan) {
    Token op = c.next();
    switch (op.kind) {
    case Token::ASSIGN:
        return _intoAssign(diag, lhs, try$(_parseExpr(c, diag, Prec::ASSIGN)), lhsSpan);

    case Token::OR:
        return opNew<OrExpr>(lhs, try$(_parseExpr(c, diag, Prec::OR)));
    case Token::AND:
        return opNew<AndExpr>(lhs, try$(_parseExpr(c, diag, Prec::AND)));

    case Token::EQ:
        return opNew<EqExpr>(lhs, try$(_parseExpr(c, diag, Prec::EQUALITY)));
    case Token::NEQ:
        return opNew<NEqExpr>(lhs, try$(_parseExpr(c, diag, Prec::EQUALITY)));

    case Token::LT:
        return opNew<LtExpr>(lhs, try$(_parseExpr(c, diag, Prec::COMPARISON)));
    case Token::LTEQ:
        return opNew<LtEqExpr>(lhs, try$(_parseExpr(c, diag, Prec::COMPARISON)));
    case Token::GT:
        return opNew<GtExpr>(lhs, try$(_parseExpr(c, diag, Prec::COMPARISON)));
    case Token::GTEQ:
        return opNew<GtEqExpr>(lhs, try$(_parseExpr(c, diag, Prec::COMPARISON)));

    case Token::PLUS:
        return opNew<AddExpr>(lhs, try$(_parseExpr(c, diag, Prec::TERM)));
    case Token::MINUS:
        return opNew<SubExpr>(lhs, try$(_parseExpr(c, diag, Prec::TERM)));
    case Token::STAR:
        return opNew<MulExpr>(lhs, try$(_parseExpr(c, diag, Prec::FACTOR)));
    case Token::SLASH:
        return opNew<DivExpr>(lhs, try$(_parseExpr(c, diag, Prec::FACTOR)));
    case Token::PERCENT:
        return opNew<ModExpr>(lhs, try$(_parseExpr(c, diag, Prec::FACTOR)));

    case Token::DOT:
        return opNew<GetExpr>(lhs, try$(opNew<QuoteExpr>(try$(_parseIdentOrValue(c, diag)))));
    case Token::LPAREN:
        return _parseCall(c, diag, lhs, op);
    case Token::LBRACKET:
        return _parseIndex(c, diag, lhs, op);
    case Token::IS:
        return opNew<IsExpr>(lhs, try$(_parseExpr(c, diag, Prec::TYPE)));
    case Token::AS:
        return opNew<AsExpr>(lhs, try$(_parseExpr(c, diag, Prec::TYPE)));

    default:
        return diag.unexpected(op, "infix position"s);
    }
}

static CompletionOr<Value> _parseExpr(Cursor<Token>& c, DiagCollector& diag, Prec minPrec) {
    Io::LocSpan lhsSpan = c->span;
    auto lhs = try$(_parsePrefix(c, diag));
    // Update lhsSpan to include the full prefix expression
    // The end is now the start of the current token (which is after the prefix)
    if (not c.ended()) {
        lhsSpan.end = c->span.start;
        // lhsSpan.text = ""s; // Clear text since span now covers multiple tokens
    }
    while (not c.ended()) {
        Prec nextPrec = _peekPrec(c);
        if (nextPrec <= minPrec)
            break;
        lhs = try$(_parseInfix(c, diag, lhs, lhsSpan));
    }
    return Ok(lhs);
}

static CompletionOr<Value> _parseTopLevel(Cursor<Token>& c, DiagCollector& diag) {
    Vec<Value> exprs;
    do {
        if (c.skip(Token::EOF))
            break;
        exprs.pushBack(try$(_parseExpr(c, diag, Prec::LOWEST)));
    } while (c.skip(Token::SEMICOLON) or *c != Token::EOF);

    return opNew<BlockExpr>(exprs, false);
}

export CompletionOr<Value> parse(Str code, DiagCollector& diag) {
    Io::SScan s{code};
    auto tokensRes = lex(s, diag);
    if (not tokensRes) {
        return tokensRes.none();
    }

    auto tokens = tokensRes.take();
    Cursor<Token> c{tokens};
    return _parseTopLevel(c, diag);
}

} // namespace Luna
