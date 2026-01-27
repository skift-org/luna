#include <karm/entry>

import Luna;
import Karm.Cli;
import Karm.Sys;
import Karm.Ref;
import Karm.Logger;

using namespace Karm;

Async::Task<> entryPointAsync(Sys::Context& ctx, Async::CancellationToken) {
    auto scriptArg = Cli::operand<Str>("script"s, "Script to run"s);

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

    if (scriptArg.value()) {
        auto url = Ref::parseUrlOrPath(scriptArg.value());
        auto code = co_try$(Sys::readAllUtf8(url));

        Luna::DiagCollector diag{code};
        auto parseRes = Luna::parse(code, diag);
        if (not parseRes) {
            diag.dumpTo(Sys::err());
            co_return Error::invalidInput("parser error");
        }

        auto evalRes = Luna::opEval(parseRes.take(), Luna::builtins().take());
        if (not evalRes) {
            auto completion = evalRes.none();
            if (completion.type == Luna::Completion::EXCEPTION) {
                logError("runtime error {}: {}", scriptArg.value(), evalRes.none().value);
                co_return Error::invalidInput("runtime error");
            }

            evalRes = Ok(completion.value);
        }

        co_return Ok();
    }

    auto env = Luna::builtins().take();
    while (true) {
        Sys::print(">>> ");
        auto line = co_try$(Io::readLineUtf8(Sys::in()));
        Luna::DiagCollector diag{line};
        auto parseRes = Luna::parse(line, diag);
        if (not parseRes) {
            diag.dumpTo(Sys::err());
            continue;
        }

        auto evalRes = Luna::opEval(parseRes.take(), env);
        if (not evalRes) {
            auto completion = evalRes.none();
            if (completion.type == Luna::Completion::EXCEPTION) {
                Sys::errln("runtime error: {}", completion.value);
            } else {
                Sys::println("{}", completion.value);
            }
        } else {
            Sys::println("{}", evalRes.take());
        }
    }

    co_return Ok();
}
