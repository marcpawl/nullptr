#include "marcpawl/pointers/ptr.hpp"
#include <catch2/catch_test_macros.hpp>

namespace mp = marcpawl::pointers;

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)

TEST_CASE("owner_not_null explicit constructor", "[owner_not_null]")
{
  SECTION("from nullptr")
  {
    try {
      mp::owner_not_null<int *> const owner{ static_cast<int *>(nullptr) };
      REQUIRE(owner.get() == nullptr);
      FAIL("Expected exception");
    } catch (mp::nullptr_exception const &e) {
      REQUIRE(e.what() == std::string(""));
    }
  }
}


// NOLINTEND (cppcoreguidelines-avoid-magic-numbers)
