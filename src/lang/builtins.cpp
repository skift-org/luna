module;

#include <karm-core/macros.h>

export module Luna:builtins;

import Karm.Sys;
import :base;
import :ops;
import :objects;

namespace Luna {

static CompletionOr<Value> _builtinLen(Reference params) {
    auto of = try$(opGet(params, "of"_sym));
    return opLen(of);
}

static CompletionOr<Value> _builtinPrint(Reference params) {
    auto fmt = try$(opGet(params, "fmt"_sym));
    Sys::println("{}", fmt);
    return Ok();
}

static CompletionOr<Value> _builtinInput(Reference params) {
    auto prompt = try$(opGet(params, "prompt"_sym));
    Sys::print("{}", prompt);
    auto line = Io::readLineUtf8(Sys::in()).take();
    return Ok(line);
}

static CompletionOr<Value> _builtinExit(Reference) {
    Sys::exit(Ok());
    return Ok();
}

export CompletionOr<Reference> builtins() {
    Reference env = try$(Environment::create(NONE));

    try$(env->decl(
        "len"_sym,
        try$(
            Func::create(
                env,
                {
                    {
                        "of"_sym,
                    },
                },
                Native{_builtinLen}
            )
        )
    ));

    try$(env->decl(
        "println"_sym,
        try$(
            Func::create(
                env,
                {
                    {
                        "fmt"_sym,
                    },
                },
                Native{_builtinPrint}
            )
        )
    ));

    try$(env->decl(
        "input"_sym,
        try$(
            Func::create(
                env,
                {
                    {
                        "prompt"_sym,
                    },
                },
                Native{_builtinInput}
            )
        )
    ));

    try$(env->decl(
        "exit"_sym,
        try$(
            Func::create(
                env,
                {},
                Native{_builtinExit}
            )
        )
    ));

    return Ok(env);
}

} // namespace Luna
