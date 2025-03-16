#include "marcpawl/pointers/ptr.hpp"
#include <catch2/catch_test_macros.hpp>

namespace mp = marcpawl::pointers;

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)

TEST_CASE("borrower_not_null explicit constructor", "[borrower_not_null]")
{
  SECTION("from nullptr")
  {
    try {
      int 
      mp::borrower<mp::strict_not_null<int *>> const borrower =
        mp::make_borrower<mp::strict_not_null<int *>>(nullptr);
      REQUIRE(borrower.get() == nullptr);
      FAIL("Expected exception");
    } catch (mp::nullptr_exception const &e) {
      REQUIRE(e.what() == std::string(""));
    }
  }
}


// NOLINTEND (cppcoreguidelines-avoid-magic-numbers)
