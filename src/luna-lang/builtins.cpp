module;

#include <karm-core/macros.h>

export module Luna:builtins;

import :base;
import :ops;
import :objects;

namespace Luna {

static CompletionOr<Value> _builtinLen(Reference params) {
    return opLen(try$(opGet(params, Symbols::OF)));
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
                        Symbols::OF,
                    },
                },
                Native{_builtinLen}
            )
        )
    ));

    return Ok(env);
}

} // namespace Luna