#include "marcpawl/pointers/ptr.hpp"

namespace mp = marcpawl::pointers;

auto fail()
{
    // Compile time failure expected
    mp::borrower<mp::strict_not_null<int*>> p;
    return p;
}
