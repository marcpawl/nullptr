#include "marcpawl/pointers/ptr.hpp"

auto fail()
{
    // Compile time failure expected
    return marcpawl::pointers::borrower<int>(5);
}
