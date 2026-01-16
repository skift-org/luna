#include <karm-test/macros.h>

import Luna;
import Karm.Test;

using namespace Karm;

namespace Luna::Tests {

test$("luna-run-all") {
    auto testsDir = try$(Sys::Dir::open("bundle://luna-lang.tests/"_url));
    for (auto& e : testsDir.entries()) {
        testsDir.path()
    }
    auto table = try$(Table::create());

    try$(table->set(
        Text::create("gay"s),
        Text::create("Clem & Mathilde"s)
    ));

    auto gay = table->get(Text::create("gay"s));

    return Ok();
}

} // namespace Luna::Tests