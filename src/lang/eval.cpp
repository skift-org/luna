module;

#include <karm-core/macros.h>

export module Luna:eval;

import :parser;
import :ops;
import :builtins;

namespace Luna {

export CompletionOr<Value> evalStr(Str code) {
    auto expr = try$(Luna::parse(code));
    auto res = opEval(expr, try$(Luna::builtins()));
    if (res)
        return res;

    auto completion = res.none();
    if (completion.type == Completion::EXCEPTION)
        return completion;
    return Ok(completion.value);
}

} // namespace Luna