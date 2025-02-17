#include "marcpawl/pointers/pointers.hpp"

auto fail()
{
    return marcpawl::pointers::borrower<int>(5);
}
