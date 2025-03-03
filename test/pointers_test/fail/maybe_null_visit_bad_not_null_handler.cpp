#include "marcpawl/pointers/ptr.hpp"

void fail()
{
    // Compile time failure expected
    // not null pointer handler
    marcpawl::pointers::maybe_null<int*> maybe(nullptr);
    auto null_handler = [](auto&&){};
    auto ptr_handler = [](int){};
    maybe.visit(null_handler, ptr_handler);
}
