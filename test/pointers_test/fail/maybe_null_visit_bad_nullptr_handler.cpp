#include "marcpawl/pointers/ptr.hpp"

auto fail()
{
    // Compile time failure expected
    // null pointer handler
    marcpawl::pointers::maybe_null<int*> maybe(nullptr);
    auto null_handler = [](){};
    auto ptr_handler = [](auto&&){};
    maybe.visit(null_handler, ptr_handler);
}
