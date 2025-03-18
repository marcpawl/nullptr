#include "hierarchy.hpp"
#include "marcpawl/pointers/ptr.hpp"
#include <catch2/catch_test_macros.hpp>

#include <gsl/gsl>

namespace mp = marcpawl::pointers;

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)

TEST_CASE("owner default constructor", "[owner]")
{
  mp::owner<int *> const owner{};
  int *actual = owner.get();
  bool result = (actual == nullptr);
  REQUIRE(result);
}

TEST_CASE("make_owner", "[owner]")
{
  SECTION("pointer")
  {
    gsl::owner<Child *> owner0{ new Child() };
    int *expected = owner0->value;
    mp::owner<Parent *> const owner = mp::make_owner<Parent *>(owner0);
    int *actual = owner->value;
    bool result = (actual == expected);
    REQUIRE(result);
    delete owner0;
  }
  SECTION("nullptr")
  {
// TODO
#if 0
    mp::owner<Parent *> const owner = mp::make_owner<Parent *>(nullptr);
    REQUIRE(owner == nullptr);
#endif
  }
}

// NOLINTEND (cppcoreguidelines-avoid-magic-numbers)
