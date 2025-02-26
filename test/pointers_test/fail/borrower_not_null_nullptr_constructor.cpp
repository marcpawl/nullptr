#include "marcpawl/pointers/ptr.hpp"

auto fail()
{
    // Compile time failure expected
    return marcpawl::pointers::borrower_not_null<int*>(nullptr);
}
