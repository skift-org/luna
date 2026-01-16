module;

#include <karm-core/macros.h>

export module Luna:eval;

import :parser;
import :ops;
import :builtins;

namespace Luna {

export CompletionOr<Value> evalStr(Str code, Reference env) {
    auto expr = try$(Luna::parse(code));
    auto res = opEval(expr, env);
    if (res)
        return res;

    auto completion = res.none();
    if (completion.type == Completion::EXCEPTION)
        return completion;
    return Ok(completion.value);
}

export CompletionOr<Value> evalStr(Str code) {
    return evalStr(code, try$(builtins()));
}

} // namespace Luna