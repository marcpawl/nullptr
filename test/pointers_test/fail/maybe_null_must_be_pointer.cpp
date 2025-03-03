#include "marcpawl/pointers/ptr.hpp"

auto fail()
{
    // Compile time failure expected
    // Must be pointer
    return marcpawl::pointers::maybe_null<int>(5);
}
