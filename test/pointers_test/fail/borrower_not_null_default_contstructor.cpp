#include "marcpawl/pointers/pointers.hpp"

auto fail()
{
    // Compile time failure expected
    return marcpawl::pointers::borrower_not_null<int*>();
}
