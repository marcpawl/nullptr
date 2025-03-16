#include "marcpawl/pointers/ptr.hpp"

namespace mp = marcpawl::pointers;

auto fail()
{
    int p;
    mp::borrower<int*> ptr{&p};
    // ERROR: cannot create an owner from a borrower.
    mp::owner<int*> owner{ptr};
    return owner;
}

int main(int,char**) {
    fail();
    return 0;
}