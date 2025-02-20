#include "marcpawl/pointers/pointers.hpp"

auto fail()
{
    // Compile time failure expected
     marcpawl::pointers::borrower<int*> borrower(new int(3));
     delete borrower.get();
}
