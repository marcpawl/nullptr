#include "marcpawl/pointers/pointers.hpp"
#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include "hierarchy.hpp"

namespace mp = marcpawl::pointers;

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)

TEST_CASE("borrower_not_null explicit constructor", "[borrower_not_null]")
{
  SECTION("from nullptr")
  {
    try {
    mp::borrower_not_null<int *> const borrower{ static_cast<int*>(nullptr) };
    //REQUIRE(borrower.get() == nullptr);
    //FAIL("Expected exception");
    } catch (mp::nullptr_exception const& e) {
     // REQUIRE(e.what() == std::string(""));
    }
  }
}


// NOLINTEND (cppcoreguidelines-avoid-magic-numbers)
