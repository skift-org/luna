#include <karm-sys/entry.h>

import Luna;
import Karm.Cli;
import Karm.Sys;
import Karm.Ref;
import Karm.Logger;

using namespace Karm;

Async::Task<> entryPointAsync(Sys::Context& ctx, Async::CancellationToken) {
    auto scriptArg = Cli::operand<Ref::Url>("script"s, "Script to run"s);

    Cli::Command cmd{
        "luna"s,
        "A scripting language"s,
        {
            Cli::Section{"Input"s, {scriptArg}},
        }
    };

    co_trya$(cmd.execAsync(ctx));
    if (not cmd)
        co_return Ok();

    auto code = co_try$(Sys::readAllUtf8(scriptArg.value()));
    Sys::println("--- code ---\n{}", code);

    auto parseRes = Luna::parse(code);
    if (not parseRes) {
        logError("failed to parse {}: {}", scriptArg.value(), parseRes.none().value);
        co_return Error::invalidInput("parser error");
    }

    Sys::println("--- exprs ---\n{}", parseRes.unwrap());

    auto evalRes = Luna::opEval(parseRes.take(), Luna::builtins().take());
    if (not evalRes) {
        logError("runtime error {}: {}", scriptArg.value(), evalRes.none().value);
        co_return Error::invalidInput("runtime error");
    }

    Sys::println("--- result ---\n{}", evalRes.unwrap());
    co_return Ok();
}
