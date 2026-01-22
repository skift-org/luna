#include <karm-test/macros.h>

import Luna;
import Karm.Test;
import Karm.Sys;
import Karm.Ref;
import Karm.Logger;

using namespace Karm;

namespace Luna::Tests {

test$("luna eval") {
    auto testsDir = try$(Sys::Dir::open("bundle://luna-lang.tests/"_url));
    for (auto& i : testsDir.entries()) {
        auto subDir = try$(Sys::Dir::open(testsDir.url() / i.name));
        for (auto& j : subDir.entries()) {
            auto url = subDir.url() / j.name;
            auto code = try$(Sys::readAllUtf8(url));
            auto result = Luna::evalStr(code);
            if (not result) {
                logError("{} failed: {}", url, result.none().value);
                return Error::other("exception occured");
            }

            expectEq$(result.unwrap(), "pass"_sym);
        }
    }

    return Ok();
}

} // namespace Luna::Tests
