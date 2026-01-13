#include <karm-test/macros.h>

import Luna;
import Karm.Test;

using namespace Karm;

namespace Luna::Tests {

test$("table-get-set") {
    auto table = try$(Table::create());

    try$(table->set(
        Text::create("gay"s),
        Text::create("Clem & Mathilde"s)
    ));

    auto gay = table->get(Text::create("gay"s));

    return Ok();
}

} // namespace Luna::Tests