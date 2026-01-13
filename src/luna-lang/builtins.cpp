module;

#include <karm-core/macros.h>

export module Luna:builtins;

import :base;
import :ops;
import :objects;

namespace Luna {

static CompletionOr<Value> _builtinBoolean(Object params) {
    return asBoolean(try$(opGet(params, Symbols::FROM)));
}

static CompletionOr<Value> _builtinInteger(Object params) {
    return asInteger(try$(opGet(params, Symbols::FROM)));
}

static CompletionOr<Value> _builtinNumber(Object params) {
    return asNumber(try$(opGet(params, Symbols::FROM)));
}

static CompletionOr<Value> _builtinSymbol(Object params) {
    return asSymbol(try$(opGet(params, Symbols::FROM)));
}

static CompletionOr<Value> _builtinString(Object params) {
    return asString(try$(opGet(params, Symbols::FROM)));
}

export CompletionOr<Object> builtins() {
    Object env = try$(Environment::create(NONE));

    try$(env->decl(
        "boolean"_sym,
        try$(
            Func::create(
                env,
                {
                    {
                        Symbols::FROM,
                    },
                },
                Native{_builtinBoolean}
            )
        )
    ));

    try$(env->decl(
        "integer"_sym,
        try$(
            Func::create(
                env,
                {
                    {
                        Symbols::FROM,
                    },
                },
                Native{_builtinInteger}
            )
        )
    ));

    try$(env->decl(
        "number"_sym,
        try$(
            Func::create(
                env,
                {
                    {
                        Symbols::FROM,
                    },
                },
                Native{_builtinNumber}
            )
        )
    ));

    try$(env->decl(
        "symbol"_sym,
        try$(
            Func::create(
                env,
                {
                    {
                        Symbols::FROM,
                    },
                },
                Native{_builtinSymbol}
            )
        )
    ));

    try$(env->decl(
        "string"_sym,
        try$(
            Func::create(
                env,
                {
                    {
                        Symbols::FROM,
                    },
                },
                Native{_builtinString}
            )
        )
    ));

    return Ok(env);
}

} // namespace Luna