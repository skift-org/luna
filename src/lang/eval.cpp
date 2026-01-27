module;

#include <karm/macros>

export module Luna:eval;

import :parser;
import :ops;
import :builtins;

namespace Luna {

export CompletionOr<Value> evalStr(Str code, Reference env, DiagCollector& diag) {
    auto expr = try$(Luna::parse(code, diag));
    auto res = opEval(expr, env);
    if (res)
        return res;

    auto completion = res.none();
    if (completion.type == Completion::EXCEPTION)
        return completion;
    return Ok(completion.value);
}

export CompletionOr<Value> evalStr(Str code, Reference env) {
    DiagCollector diag{code};
    return evalStr(code, env, diag);
}

export CompletionOr<Value> evalStr(Str code) {
    return evalStr(code, try$(builtins()));
}

} // namespace Luna
