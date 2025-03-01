#include <cassert>

#include "hierarchy.hpp"
#include "marcpawl/pointers/ptr.hpp"
#include <catch2/catch_test_macros.hpp>
// #include <sstream>

namespace mp = marcpawl::pointers;

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)

static void doubled(int *const data)
{
  assert(data != nullptr);
  (*data) = 2 * (*data);
}

TEST_CASE("operator T", "[not_null]")
{
  int data = 32;
  mp::maybe_null<int *> maybe_null(&data);
  auto opt = maybe_null.as_not_null();
  REQUIRE(opt.has_value());
  mp::strict_not_null<int *> const &not_null = opt.value();
  REQUIRE(data == *not_null);
  doubled(not_null);
  REQUIRE(64 == *not_null);
}

TEST_CASE("copy constructor", "[not_null]")
{
  SECTION("same types")
  {
    int const data = 4;
    mp::maybe_null<int const *> const source{ &data };
    mp::maybe_null<int const *> const destination{ source };
    auto opt = destination.as_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<int const *> const &not_null = opt.value();
    REQUIRE(data == *not_null);
  }
  SECTION("inherited types")
  {
    int *data = new int{ 4 };
    Child child(data);
    mp::maybe_null<Child *> const source{ &child };
    mp::maybe_null<Parent *> const destination{ source };
    auto opt = destination.as_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<Parent *> const &not_null = opt.value();
    int const actual = not_null->get_value();
    REQUIRE(*data == actual);
  }
}

TEST_CASE("move constructor", "[not_null]")
{
  SECTION("same types")
  {
    int const data = 4;
    mp::maybe_null<int const *> const source{ &data };
    mp::maybe_null<int const *> const destination{ std::move(source) };
    auto opt = destination.as_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<int const *> const &not_null = opt.value();
    REQUIRE(data == *not_null);
  }
  SECTION("inherited types")
  {
    int *data = new int{ 4 };
    Child child(data);
    mp::maybe_null<Child *> const source{ &child };
    mp::maybe_null<Parent *> const destination{ std::move(source) };
    auto opt = destination.as_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<Parent *> const &not_null = opt.value();
    int const actual = not_null->get_value();
    REQUIRE(*data == actual);
  }
}


// NOLINTEND (cppcoreguidelines-avoid-magic-numbers)
