#include "marcpawl/pointers/pointers.hpp"

auto fail()
{
    // Compile time failure expected
    return marcpawl::pointers::borrower<int>(5);
}
