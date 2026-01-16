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
        "print"_sym,
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

    return Ok(env);
}

} // namespace Luna
