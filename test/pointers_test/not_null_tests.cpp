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


TEST_CASE("copy constructor", "[maybe_null]")
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

#if 0
TEST_CASE("move constructor", "[maybe_null]")
{
  SECTION("from parent")
  {
    gsl::owner<Parent *> owner{ new Parent() };
    mp::maybe_null<Parent *> const parent1{ owner };
    // NOLINTNEXTLINE (hicpp-move-const-arg,performance-move-const-arg)
    // mp::maybe_null<Parent *> const parent2( std::move(parent1) );
    // REQUIRE(parent2.get() != nullptr);
    delete owner;
  }
  SECTION("from child")
  {
    // NOLINTNEXTLINE (hicpp-move-const-arg,performance-move-const-arg)
    gsl::owner<Child *> owner(new Child());
    mp::maybe_null<Child *> const child1(owner);
    // NOLINTNEXTLINE (hicpp-move-const-arg,performance-move-const-arg)
    // mp::maybe_null<Parent *> parent3( std::move(child1) );
    // REQUIRE(parent3.get() != nullptr);
    delete owner;
  }
}

TEST_CASE("assignment", "[maybe_null]")
{
  SECTION("from parent")
  {
    gsl::owner<Parent *> owner0{ new Parent() };
    gsl::owner<Parent *> owner1{ new Parent() };
    mp::maybe_null<Parent *> parent1(owner0);
    mp::maybe_null<Parent *> const parent2(owner1);
    parent1 = parent2;
    REQUIRE(owner1 == parent1.get());
    REQUIRE(owner1 == parent2.get());
    delete owner1;
    delete owner0;
  }
  SECTION("from child")
  {
    // NOLINTNEXTLINE (hicpp-use-auto,modernize-use-auto)
    gsl::owner<Child *> owner2(new Child());
    // NOLINTNEXTLINE (hicpp-use-auto,modernize-use-auto)
    gsl::owner<Child *> const owner3(new Child());
    mp::maybe_null<Child *> child1(owner2);
    mp::maybe_null<Child *> const child2(owner3);
    child1 = child2;
    REQUIRE(child1.get() == owner3);
    REQUIRE(child2.get() == owner3);
    delete owner3;
    delete owner2;
  }
}

TEST_CASE("move assignment", "[maybe_null]")
{
  SECTION("from parent")
  {
    gsl::owner<Parent *> owner0{ new Parent() };
    gsl::owner<Parent *> owner1{ new Parent() };
    mp::maybe_null<Parent *> parent1(owner0);
    mp::maybe_null<Parent *> const parent2(owner1);
    // NOLINTNEXTLINE (hicpp-move-const-arg,performance-move-const-arg)
    parent1 = std::move(parent2);
    REQUIRE(owner1 == parent1.get());
    delete owner1;
    delete owner0;
  }
  SECTION("from child")
  {
    // NOLINTNEXTLINE (hicpp-use-auto,modernize-use-auto)
    gsl::owner<Child *> owner2(new Child());
    // NOLINTNEXTLINE (hicpp-use-auto,modernize-use-auto)
    gsl::owner<Child *> const owner3(new Child());
    mp::maybe_null<Child *> child1(owner2);
    mp::maybe_null<Child *> const child2(owner3);
    // NOLINTNEXTLINE (hicpp-move-const-arg,performance-move-const-arg)
    child1 = std::move(child2);
    REQUIRE(child1.get() == owner3);
    delete owner3;
    delete owner2;
  }
}

TEST_CASE("arrow operator", "[maybe_null]")
{
  gsl::owner<Child *> owner0{ new Child() };
  mp::maybe_null<Parent *> const maybe_null = mp::make_maybe_null<Parent *>(owner0);
  REQUIRE(maybe_null->value == owner0->value);
  delete owner0;
}

TEST_CASE("dereference operator", "[maybe_null]")
{
  gsl::owner<Child *> owner0{ new Child() };
  mp::maybe_null<Parent *> const maybe_null = mp::make_maybe_null<Parent *>(owner0);
  REQUIRE((*maybe_null).value == owner0->value);
  delete owner0;
}

TEST_CASE("make_maybe_null", "[maybe_null]")
{
  gsl::owner<Child *> owner0{ new Child() };
  mp::maybe_null<Parent *> const maybe_null1 = mp::make_maybe_null<Parent *>(owner0);
  REQUIRE(maybe_null1.get() == owner0);
  mp::maybe_null<Parent *> const maybe_null2 = mp::make_maybe_null<Parent *>(owner0);
  REQUIRE(maybe_null2.get() == owner0);
  delete owner0;
}

#if !defined(GSL_NO_IOSTREAMS)
TEST_CASE("stream operator", "[maybe_null]")
{
  gsl::owner<Child *> owner0{ new Child() };
  mp::maybe_null<Parent *> const maybe_null = mp::make_maybe_null<Parent *>(owner0);
  std::ostringstream actual_stream;
  actual_stream << maybe_null;
  std::ostringstream expected_stream;
  expected_stream << maybe_null.get();
  std::string actual = actual_stream.str();
  std::string expected = expected_stream.str();
  REQUIRE(actual == expected);
  delete owner0;
}
#endif

TEST_CASE("comparison operator", "[maybe_null]")
{
  gsl::owner<Child *> owner0{ new Child() };
  mp::maybe_null<Parent *> const maybe_null0 = mp::make_maybe_null<Parent *>(owner0);
  gsl::owner<Child *> owner1{ new Child() };
  mp::maybe_null<Child *> const maybe_null1 = mp::make_maybe_null<Child *>(owner1);
  SECTION("equality")
  {
    REQUIRE(maybe_null0 == maybe_null0);
    REQUIRE_FALSE(maybe_null0 == maybe_null1);
    REQUIRE_FALSE(maybe_null1 == maybe_null0);
    REQUIRE_FALSE(maybe_null0 != maybe_null0);
    REQUIRE(maybe_null1 != maybe_null0);
    REQUIRE(maybe_null0 != maybe_null1);
    // NOLINTNEXTLINE (misc-redundant-expression)
    REQUIRE(maybe_null0 == maybe_null0);
    REQUIRE_FALSE(maybe_null0 == maybe_null1);
    REQUIRE_FALSE(maybe_null1 == maybe_null0);
    // NOLINTNEXTLINE (misc-redundant-expression)
    REQUIRE_FALSE(maybe_null0 != maybe_null0);
    REQUIRE(maybe_null1 != maybe_null0);
    REQUIRE(maybe_null0 != maybe_null1);
  }
  SECTION("less than")
  {
    bool const b0lt1 = maybe_null0 < maybe_null1;
    bool const b1lt0 = maybe_null1 < maybe_null0;
    bool const blt = b0lt1 || b1lt0;
    REQUIRE(blt);
    REQUIRE(b0lt1 ^ b1lt0);
  }
  SECTION("greater than")
  {
    bool const b0gt1 = maybe_null0 > maybe_null1;
    bool const b1gt0 = maybe_null1 > maybe_null0;
    bool const bgt = b0gt1 || b1gt0;
    REQUIRE(bgt);
    REQUIRE(b0gt1 ^ b1gt0);
  }
  SECTION("less than equal")
  {
    bool const b0lte1 = maybe_null0 <= maybe_null1;
    bool const b1lte0 = maybe_null1 <= maybe_null0;
    bool const blte = b0lte1 || b1lte0;
    REQUIRE(blte);
    REQUIRE(b0lte1 ^ b1lte0);
    REQUIRE(maybe_null0 <= maybe_null0);
  }
  SECTION("greater than equal")
  {
    bool const b0gte1 = maybe_null0 >= maybe_null1;
    bool const b1gte0 = maybe_null1 >= maybe_null0;
    bool const bgte = b0gte1 || b1gte0;
    REQUIRE(bgte);
    REQUIRE(b0gte1 ^ b1gte0);
    REQUIRE(maybe_null0 <= maybe_null0);
  }
  SECTION("nullptr")
  {
    mp::maybe_null<int *> null_maybe_null{ nullptr };
    REQUIRE(null_maybe_null == nullptr);
    REQUIRE_FALSE(maybe_null0 == nullptr);
    REQUIRE(nullptr == null_maybe_null);
    REQUIRE_FALSE(nullptr == maybe_null0);

    REQUIRE_FALSE(null_maybe_null != nullptr);
    REQUIRE(maybe_null0 != nullptr);
    REQUIRE_FALSE(nullptr != null_maybe_null);
    REQUIRE(nullptr != maybe_null0);
  }
  SECTION("0 as nullptr")
  {
    mp::maybe_null<int *> null_maybe_null{ nullptr };
    // TODO REQUIRE(null_maybe_null == 0);
    // TODO REQUIRE_FALSE(maybe_null0 == 0);
    // TODO REQUIRE(0 == null_maybe_null);
    // TODO REQUIRE_FALSE(0 == maybe_null0);
  }
  delete owner1;
  delete owner0;
}

TEST_CASE("nullable not_null comparison operator", "[maybe_null]")
{
  int const a = 21;
  int const b = 22;
  auto nullable_a = mp::make_maybe_null<int const *>(&a);
  auto not_null_a = mp::make_maybe_null_not_null<int const *>(&a);
  auto not_null_b = mp::make_maybe_null_not_null<int const *>(&b);
  SECTION("operator==")
  {
    REQUIRE(nullable_a == not_null_a);
    REQUIRE(not_null_a == nullable_a);
    REQUIRE(nullable_a <= not_null_a);
    REQUIRE(not_null_a >= nullable_a);
  }
  SECTION("operator!=")
  {
    REQUIRE(nullable_a != not_null_b);
    REQUIRE(not_null_b != nullable_a);
  }
  SECTION("inequality")
  {
    if (nullable_a < not_null_b) {
      REQUIRE(nullable_a <= not_null_b);
      REQUIRE(not_null_b > nullable_a);
      REQUIRE(not_null_b >= nullable_a);
    } else {
      REQUIRE(false);// TODO
    }
  }
}

TEST_CASE("hashing", "[maybe_null]")
{
  gsl::owner<Child *> owner0{ new Child() };
  mp::maybe_null<Parent *> const maybe_null0 = mp::make_maybe_null<Parent *>(owner0);
  auto expected = std::hash<Child *>{}(owner0);
  auto actual = std::hash<mp::maybe_null<Parent *>>{}(maybe_null0);
  REQUIRE(actual == expected);
  delete owner0;
}

void not_null_maybe_null(gsl::not_null<mp::maybe_null<int *>> maybe_null)
{
  *maybe_null = 43;
}

TEST_CASE("not_null", "[maybe_null]")
{
  std::unique_ptr<int> owner = std::make_unique<int>(42);
  not_null_maybe_null(mp::make_maybe_null<int *>(owner.get()));
  REQUIRE(*owner == 43);
}

void reference(int &maybe_null) { maybe_null = 43; }

TEST_CASE("reference", "[maybe_null]")
{
  SECTION("not_null")
  {
    std::unique_ptr<int> owner = std::make_unique<int>(42);
    auto maybe_null = gsl::make_not_null(mp::make_maybe_null<int *>(owner.get()));
    reference(*maybe_null);
    REQUIRE(*owner == 43);
  }
  SECTION("strict_not_null")
  {
    std::unique_ptr<int> owner = std::make_unique<int>(44);
    auto maybe_null =
      gsl::make_strict_not_null(mp::make_maybe_null<int *>(owner.get()));
    reference(*maybe_null);
    REQUIRE(*owner == 43);
  }
}


TEST_CASE("bool", "[maybe_null]")
{
  SECTION("not_null")
  {
    std::unique_ptr<int> owner = std::make_unique<int>(42);
    mp::maybe_null<int *> maybe_null = mp::make_maybe_null<int *>(owner.get());
    bool const not_null = maybe_null;
    REQUIRE(not_null);
  }
  SECTION("null")
  {
    mp::maybe_null<int *> maybe_null = mp::make_maybe_null<int *>(nullptr);
    bool const null = maybe_null;
    REQUIRE_FALSE(null);
  }
}

TEST_CASE("void*", "[maybe_null]")
{
  SECTION("not_null")
  {
    std::unique_ptr<int> owner = std::make_unique<int>(42);
    mp::maybe_null<int *> maybe_null = mp::make_maybe_null<int *>(owner.get());
    void *not_null = maybe_null.get();
    REQUIRE(not_null != nullptr);
  }
  SECTION("null")
  {
    mp::maybe_null<int *> maybe_null = mp::make_maybe_null<int *>(nullptr);
    void *null = maybe_null.get();
    REQUIRE(null == nullptr);
  }
}

TEST_CASE("gsl::not_null", "[maybe_null]")
{
  std::unique_ptr<int> owner = std::make_unique<int>(42);
  mp::maybe_null<int *> maybe_null = mp::make_maybe_null<int *>(owner.get());
  auto var_not_null_maybe_null = gsl::make_not_null(maybe_null);
  REQUIRE(var_not_null_maybe_null.get() == maybe_null);
  REQUIRE(var_not_null_maybe_null.get().get() == owner.get());

  REQUIRE(42 == legacy(var_not_null_maybe_null.get().get()));
}
#endif

// NOLINTEND (cppcoreguidelines-avoid-magic-numbers)
