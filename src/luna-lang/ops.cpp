module;

#include <karm-core/macros.h>

export module Luna:ops;

import :base;

namespace Luna {

// MARK: TypeOf ----------------------------------------------------------------

Symbol typeOf(Value v) {
    return v.visit(Visitor{
        [](None) {
            return Symbols::NONE;
        },
        [](Boolean) {
            return Symbols::BOOLEAN;
        },
        [](Integer) {
            return Symbols::INTEGER;
        },
        [](Number) {
            return Symbols::NUMBER;
        },
        [](Symbol) {
            return Symbols::SYMBOL;
        },
        [](String) {
            return Symbols::STRING;
        },
        [](Object) {
            return Symbols::OBJECT;
        },
    });
}

// MARK: Is --------------------------------------------------------------------

Boolean isNone(Value v) {
    return v.is<None>();
}

Boolean isBoolean(Value v) {
    return v.is<Boolean>();
}

Boolean isInteger(Value v) {
    return v.is<Integer>();
}

Boolean isIndex(Value v) {
    return isInteger(v);
}

Boolean isNumber(Value v) {
    return v.is<Number>();
}

Boolean isScalar(Value v) {
    return isNone(v) or isBoolean(v) or isInteger(v) or isNumber(v);
}

Boolean isSymbol(Value v) {
    return v.is<Symbol>();
}

Boolean isString(Value v) {
    return v.is<String>();
}

Boolean isObject(Value v) {
    return v.is<Object>();
}

Boolean is(Value v, Symbol type) {
    if (type == Symbols::NONE)
        return isNone(v);
    if (type == Symbols::BOOLEAN)
        return isBoolean(v);
    if (type == Symbols::INTEGER)
        return isInteger(v);
    if (type == Symbols::NUMBER)
        return isNumber(v);
    if (type == Symbols::SYMBOL)
        return isSymbol(v);
    if (type == Symbols::STRING)
        return isString(v);
    if (type == Symbols::OBJECT)
        return isObject(v);
    return false;
}

// MARK: As --------------------------------------------------------------------

CompletionOr<None> asNone(Value v) {
    return v.visit(Visitor{
        [](None) -> CompletionOr<None> {
            return Ok(NONE);
        },
        [](auto) -> CompletionOr<None> {
            return Completion::exception("could not convert to none");
        },
    });
}

CompletionOr<Boolean> asBoolean(Value v) {
    return v.visit(Visitor{
        [](None) -> CompletionOr<Boolean> {
            return Ok(false);
        },
        [](Boolean b) -> CompletionOr<Boolean> {
            return Ok(b);
        },
        [](Integer i) -> CompletionOr<Boolean> {
            return Ok(static_cast<Boolean>(i));
        },
        [](Number n) -> CompletionOr<Boolean> {
            return Ok(static_cast<Boolean>(n));
        },
        [](Symbol) -> CompletionOr<Boolean> {
            return Ok(true);
        },
        [](String s) -> CompletionOr<Boolean> {
            return Ok(s.len() > 0);
        },
        [](Object o) -> CompletionOr<Boolean> {
            return o->boolean();
        },
    });
}

CompletionOr<Integer> asInteger(Value v) {
    return v.visit(Visitor{
        [](None) -> CompletionOr<Integer> {
            return Ok(0);
        },
        [](Boolean b) -> CompletionOr<Integer> {
            return Ok(b ? 1 : 0);
        },
        [](Integer i) -> CompletionOr<Integer> {
            return Ok(i);
        },
        [](Number i) -> CompletionOr<Integer> {
            return Ok(static_cast<Integer>(i));
        },
        [](auto) -> CompletionOr<Integer> {
            return Completion::exception("could not convert to integer");
        },
    });
}

CompletionOr<Integer> asIndex(Value v) {
    if (not isIndex(v))
        return Completion::exception("{} is not a valid index");
    return asInteger(v);
}

CompletionOr<Number> asNumber(Value v) {
    return v.visit(Visitor{
        [](None) -> CompletionOr<Number> {
            return Ok(0.0);
        },
        [](Boolean b) -> CompletionOr<Number> {
            return Ok(b ? 1.0 : 0.0);
        },
        [](Integer i) -> CompletionOr<Number> {
            return Ok(static_cast<Number>(i));
        },
        [](Number n) -> CompletionOr<Number> {
            return Ok(n);
        },
        [](auto) -> CompletionOr<Number> {
            return Completion::exception("could not convert to number");
        },
    });
}

CompletionOr<String> asString(Value v) {
    return v.visit(Visitor{
        [](None) -> CompletionOr<String> {
            return Ok<String>({"none"s});
        },
        [](bool b) -> CompletionOr<String> {
            return Ok<String>(b ? "true"s : "false"s);
        },
        [](Integer i) -> CompletionOr<String> {
            return Ok<String>(Io::toStr(i));
        },
        [](Number n) -> CompletionOr<String> {
            return Ok<String>(Io::toStr(n));
        },
        [](Symbol s) -> CompletionOr<String> {
            return Ok<String>(s.str());
        },
        [](String s) -> CompletionOr<String> {
            return Ok<String>(s);
        },
        [](Object o) -> CompletionOr<String> {
            auto res = try$(o->string());
            if (not isString(res))
                return Completion::exception("expected string");
            return Ok(res.take<String>());
        },
    });
}

CompletionOr<Symbol> asSymbol(Value v) {
    return v.visit(Visitor{
        [](Symbol s) -> CompletionOr<Symbol> {
            return Ok(s);
        },
        [](auto) -> CompletionOr<Symbol> {
            return Completion::exception("could not convert to symbol");
        },
    });
}

CompletionOr<Object> asObject(Value v) {
    return v.visit(Visitor{
        [](Object o) -> CompletionOr<Object> {
            return Ok(o);
        },
        [](auto) -> CompletionOr<Object> {
            return Completion::exception("could not convert to object");
        },
    });
}

CompletionOr<Value> as(Value v, Symbol type) {
    if (type == Symbols::NONE)
        return asNone(v);
    if (type == Symbols::BOOLEAN)
        return asBoolean(v);
    if (type == Symbols::INTEGER)
        return asInteger(v);
    if (type == Symbols::NUMBER)
        return asNumber(v);
    if (type == Symbols::SYMBOL)
        return asSymbol(v);
    if (type == Symbols::STRING)
        return asString(v);
    if (type == Symbols::OBJECT)
        return asObject(v);

    return Completion::exception("could not convert");
}

// MARK: Operations ------------------------------------------------------------

CompletionOr<Boolean> opEq(Value lhs, Value rhs) {
    if (auto o = lhs.is<Object>())
        return o->unwrap().eq(rhs);

    if (auto o = rhs.is<Object>())
        return o->unwrap().eq(lhs);

    if (isNumber(lhs) or isNumber(rhs))
        return Ok(try$(asNumber(lhs)) == try$(asNumber(rhs)));

    return Ok(try$(asInteger(lhs)) == try$(asInteger(rhs)));
}

static Symbol _fromOrdering(std::partial_ordering ordering) {
    if (ordering == std::partial_ordering::less)
        return Symbols::LESS;

    if (ordering == std::partial_ordering::equivalent)
        return Symbols::EQUIVALENT;

    if (ordering == std::partial_ordering::greater)
        return Symbols::GREATER;

    if (ordering == std::partial_ordering::unordered)
        return Symbols::UNORDERED;

    unreachable();
}

static Symbol _fromOrdering(std::strong_ordering ordering) {
    if (ordering == std::strong_ordering::less)
        return Symbols::LESS;

    if (ordering == std::strong_ordering::equivalent)
        return Symbols::EQUIVALENT;

    if (ordering == std::strong_ordering::greater)
        return Symbols::GREATER;

    if (ordering == std::strong_ordering::equal)
        return Symbols::EQUIVALENT;

    unreachable();
}

CompletionOr<Symbol> opCmp(Value lhs, Value rhs) {
    if (auto o = lhs.is<Object>())
        return o->unwrap().cmp(rhs);

    if (isString(lhs) or isString(rhs))
        return Ok(
            _fromOrdering(
                try$(asString(lhs)) <=>
                try$(asString(rhs))
            )
        );

    if (isNumber(lhs) or isNumber(rhs))
        return Ok(
            _fromOrdering(
                try$(asNumber(lhs)) <=>
                try$(asNumber(rhs))
            )
        );

    return Ok(
        _fromOrdering(
            try$(asInteger(lhs)) <=>
            try$(asInteger(rhs))
        )
    );
}

CompletionOr<Boolean> opAnd(Value lhs, Value rhs) {
    return Ok(
        try$(asBoolean(lhs)) and
        try$(asBoolean(rhs))
    );
}

CompletionOr<Boolean> opOr(Value lhs, Value rhs) {
    return Ok(
        try$(asBoolean(lhs)) or
        try$(asBoolean(rhs))
    );
}

CompletionOr<Boolean> opNot(Value v) {
    return Ok(
        not try$(asBoolean(v))
    );
}

CompletionOr<Value> opGet(Value val, Value key) {
    auto obj = try$(asObject(val));
    return obj->get(key);
}

CompletionOr<> opSet(Value val, Value key, Value value) {
    auto obj = try$(asObject(val));
    try$(obj->set(key, value));
    return Ok();
}

CompletionOr<Value> opDecl(Value val, Value key, Value value) {
    auto obj = try$(asObject(val));
    try$(obj->decl(key, value));
    return Ok(value);
}

CompletionOr<Boolean> opHas(Value val, Value key) {
    auto obj = try$(asObject(val));
    return obj->has(key);
}

CompletionOr<Value> opLen(Value val) {
    auto obj = try$(asObject(val));
    return obj->len();
}

CompletionOr<Value> opCall(Value val, Object params) {
    auto obj = try$(asObject(val));
    auto res = obj->call(params);
    if (res)
        return res;
    Completion completion = res.none();
    if (completion.type == Completion::EXCEPTION)
        return res;
    return Ok(completion.value);
}

CompletionOr<Value> opNeg(Value v) {
    return v.visit(Visitor{
        [](None) -> CompletionOr<Value> {
            return Ok(Integer{0});
        },
        [](Boolean b) -> CompletionOr<Value> {
            return Ok(-try$(asInteger(b)));
        },
        [](Integer i) -> CompletionOr<Value> {
            return Ok(-i);
        },
        [](Number n) -> CompletionOr<Value> {
            return Ok(-n);
        },
        [](auto&) -> CompletionOr<Value> {
            return Completion::exception("scalar operation on non scalar");
        },
    });
}

CompletionOr<Value> opAdd(Value lhs, Value rhs) {
    if (isString(lhs) or isString(rhs)) {
        return Ok(
            Io::format(
                "{}{}",
                try$(asString(lhs)),
                try$(asString(rhs))
            )
        );
    }

    if (isSymbol(lhs) or isSymbol(rhs)) {
        return Ok(
            Symbol::from(
                Io::format(
                    "{}{}",
                    try$(asString(lhs)),
                    try$(asString(rhs))
                )
            )
        );
    }

    if (lhs.is<Number>() or rhs.is<Number>())
        return Ok(
            try$(asNumber(lhs)) +
            try$(asNumber(rhs))
        );

    return Ok(
        try$(asInteger(lhs)) +
        try$(asInteger(rhs))
    );
}

CompletionOr<Value> opSub(Value lhs, Value rhs) {
    if (not isScalar(lhs))
        return Completion::exception("scalar operation on non scalar");

    if (not isScalar(rhs))
        return Completion::exception("scalar operation on non scalar");

    if (lhs.is<Number>() or rhs.is<Number>())
        return Ok(
            try$(asNumber(lhs)) -
            try$(asNumber(rhs))
        );

    return Ok(
        try$(asInteger(lhs)) -
        try$(asInteger(rhs))
    );
}

CompletionOr<Value> opMul(Value lhs, Value rhs) {
    if (not isScalar(lhs))
        return Completion::exception("scalar operation on non scalar");

    if (not isScalar(rhs))
        return Completion::exception("scalar operation on non scalar");

    if (lhs.is<Number>() or rhs.is<Number>())
        return Ok(
            try$(asNumber(lhs)) *
            try$(asNumber(rhs))
        );

    return Ok(
        try$(asInteger(lhs)) *
        try$(asInteger(rhs))
    );
}

CompletionOr<Value> opDiv(Value lhs, Value rhs) {
    if (not isScalar(lhs))
        return Completion::exception("scalar operation on non scalar");

    if (not isScalar(rhs))
        return Completion::exception("scalar operation on non scalar");

    if (lhs.is<Number>() or rhs.is<Number>())
        return Ok(
            try$(asNumber(lhs)) /
            try$(asNumber(rhs))
        );

    return Ok(
        try$(asInteger(lhs)) /
        try$(asInteger(rhs))
    );
}

CompletionOr<Value> opMod(Value lhs, Value rhs) {
    if (not isScalar(lhs))
        return Completion::exception("scalar operation on non scalar");

    if (not isScalar(rhs))
        return Completion::exception("scalar operation on non scalar");

    if (lhs.is<Number>() or rhs.is<Number>())
        return Ok(
            Math::fmod(
                try$(asNumber(lhs)),
                try$(asNumber(rhs))
            )
        );

    return Ok(
        try$(asInteger(lhs)) %
        try$(asInteger(rhs))
    );
}

CompletionOr<Value> opBinNot(Value v) {
    return Ok(~try$(asInteger(v)));
}

CompletionOr<Value> opBinAnd(Value lhs, Value rhs) {
    return Ok(try$(asInteger(lhs)) & try$(asInteger(rhs)));
}

CompletionOr<Value> opBinOr(Value lhs, Value rhs) {
    return Ok(try$(asInteger(lhs)) | try$(asInteger(rhs)));
}

template <typename T, typename... Args>
CompletionOr<Object> opNew(Args&&... args) {
    return Ok(makeRc<T>(std::forward<Args>(args)...));
}

export CompletionOr<Value> opEval(Value v, Object env) {
    return v.visit(Visitor{
        [&](auto) -> CompletionOr<Value> {
            return Ok(v);
        },
        [&](Symbol s) -> CompletionOr<Value> {
            return env->get(s);
        },
        [&](Object o) -> CompletionOr<Value> {
            return o->eval(env);
        },
    });
}

} // namespace Luna

template <>
struct Karm::Io::Formatter<Luna::Value> {
    Res<> format(TextWriter& writer, Luna::Value const& val) {
        auto res = Luna::asString(val);
        if (not res)
            return Error::other("exception occurred");
        return writer.writeStr(res.unwrap().str());
    }
};
