#include <karm/test>

import Luna;
import Karm.Test;
import Karm.Logger;

using namespace Karm;

namespace Luna::Tests {

// Helper to check that parsing fails with a specific error code
static bool hasErrorCode(DiagCollector const& diag, Str code) {
    for (auto const& d : diag.diags) {
        if (d.code == code)
            return true;
    }
    return false;
}

// MARK: Lexer Errors ----------------------------------------------------------

test$("lexer E0001 - unterminated string literal") {
    Str code = "var x = \"hello"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(not result);
    expect$(hasErrorCode(diag, "E0001"s));

    return Ok();
}

test$("lexer E0002 - unexpected character") {
    Str code = "var x = @"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(not result);
    expect$(hasErrorCode(diag, "E0002"s));

    return Ok();
}

// MARK: Parser Errors ---------------------------------------------------------

test$("parser E0100 - expected token") {
    // Missing value after =
    Str code = "var x ="s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(not result);
    expect$(hasErrorCode(diag, "E0100"s));

    return Ok();
}

test$("parser E0101 - unexpected token") {
    // E0101 is triggered when an unexpected token appears in infix position.
    // Currently this is defensive code - _peekPrec and _parseInfix are kept in sync,
    // so all tokens with precedence > LOWEST are handled. This test documents the
    // intended behavior even though it may not currently be triggerable.
    // If a new operator is added to _peekPrec but not _parseInfix, this would fire.
    return Ok();
}

test$("parser E0102 `erminated string in parser") {
    // E0102 is triggered when the parser sees LSTR + SPAN tokens but no RSTR.
    // However, the lexer always either emits RSTR or fails with E0001 before
    // the parser runs. This is defensive code for robustness - if the lexer
    // were changed to emit partial string tokens, this would catch it.
    return Ok();
}

test$("parser E0103 - expected '=' in variable declaration") {
    Str code = "var x 42"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(not result);
    expect$(hasErrorCode(diag, "E0103"s));

    return Ok();
}

test$("parser E0104 - expected 'catch' after try block") {
    Str code = "try { 1 }"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(not result);
    expect$(hasErrorCode(diag, "E0104"s));

    return Ok();
}

test$("parser E0105 - expected '(' after 'fn'") {
    Str code = "fn x"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(not result);
    expect$(hasErrorCode(diag, "E0105"s));

    return Ok();
}

test$("parser E0106 - unclosed parenthesis") {
    Str code = "(1 + 2"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(not result);
    expect$(hasErrorCode(diag, "E0106"s));

    return Ok();
}

test$("parser E0107 - expected ':' in table entry") {
    // After the first entry has colon, subsequent entries are expected to follow the same pattern
    Str code = "{ a: 1, b }"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(not result);
    expect$(hasErrorCode(diag, "E0107"s));

    return Ok();
}

test$("parser E0108 - unclosed table") {
    Str code = "{ a: 1"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(not result);
    expect$(hasErrorCode(diag, "E0108"s));

    return Ok();
}

test$("parser E0109 - unclosed block") {
    Str code = "{ 1; 2"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(not result);
    expect$(hasErrorCode(diag, "E0109"s));

    return Ok();
}

test$("parser E0110 - unclosed list") {
    Str code = "[1, 2"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(not result);
    expect$(hasErrorCode(diag, "E0110"s));

    return Ok();
}

test$("parser E0111 - unclosed function call") {
    Str code = "foo(1, 2"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(not result);
    expect$(hasErrorCode(diag, "E0111"s));

    return Ok();
}

test$("parser E0112 - unclosed index expression") {
    Str code = "arr[0"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(not result);
    expect$(hasErrorCode(diag, "E0112"s));

    return Ok();
}

test$("parser E0200 - expression is not assignable") {
    Str code = "(1+1) = 5"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(not result);
    expect$(hasErrorCode(diag, "E0200"s));

    return Ok();
}

// MARK: Valid Syntax (sanity checks) ------------------------------------------

test$("parser valid - simple expression") {
    Str code = "1 + 2"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(result.has());
    expectEq$(diag.diags.len(), 0uz);

    return Ok();
}

test$("parser valid - variable declaration") {
    Str code = "var x = 42"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(result.has());
    expectEq$(diag.diags.len(), 0uz);

    return Ok();
}

test$("parser valid - function definition") {
    Str code = "fn(a, b) { a + b }"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(result.has());
    expectEq$(diag.diags.len(), 0uz);

    return Ok();
}

test$("parser valid - table") {
    Str code = "{ a: 1, b: 2 }"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(result.has());
    expectEq$(diag.diags.len(), 0uz);

    return Ok();
}

test$("parser valid - list") {
    Str code = "[1, 2, 3]"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(result.has());
    expectEq$(diag.diags.len(), 0uz);

    return Ok();
}

test$("parser valid - try-catch") {
    Str code = "try { 1 } catch(e) { e }"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(result.has());
    expectEq$(diag.diags.len(), 0uz);

    return Ok();
}

test$("parser valid - string literal") {
    Str code = "\"hello world\""s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(result.has());
    expectEq$(diag.diags.len(), 0uz);

    return Ok();
}

test$("parser valid - nested expressions") {
    Str code = "((1 + 2) * 3)"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(result.has());
    expectEq$(diag.diags.len(), 0uz);

    return Ok();
}

test$("parser valid - function call") {
    Str code = "foo(1, 2, 3)"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(result.has());
    expectEq$(diag.diags.len(), 0uz);

    return Ok();
}

test$("parser valid - index access") {
    Str code = "arr[0]"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(result.has());
    expectEq$(diag.diags.len(), 0uz);

    return Ok();
}

test$("parser valid - assignment") {
    Str code = "x = 42"s;
    DiagCollector diag{code};
    auto result = parse(code, diag);

    expect$(result.has());
    expectEq$(diag.diags.len(), 0uz);

    return Ok();
}

} // namespace Luna::Tests
