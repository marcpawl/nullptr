#include "hierarchy.hpp"
#include "marcpawl/pointers/ptr.hpp"
#include <catch2/catch_test_macros.hpp>
// #include <sstream>

namespace mp = marcpawl::pointers;

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)

TEST_CASE("maybe_null default constructor", "[maybe_null]")
{
  mp::maybe_null<int *> const maybe_null;
  auto opt = maybe_null.as_not_null();
  REQUIRE(!opt.has_value());
}

TEST_CASE("explicit nullptr", "[maybe_null]")
{
  mp::maybe_null<int *> const maybe_null{ nullptr };
  auto opt = maybe_null.as_not_null();
  REQUIRE(!opt.has_value());
}

TEST_CASE("explicit constructor", "[maybe_null]")
{
  SECTION("from typed nullptr")
  {
    int *data = nullptr;
    mp::maybe_null<int *> const maybe_null(data);
    auto opt = maybe_null.as_not_null();
    REQUIRE(!opt.has_value());
  }
  SECTION("from non-null")
  {
    int data = 32;
    mp::maybe_null<int *> maybe_null(&data);
    auto opt = maybe_null.as_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<int *> &not_null = opt.value();
    REQUIRE(data == *not_null);
  }
  SECTION("from child")
  {
    int *parentValue = new int(32);
    std::unique_ptr<Child> data(new Child(parentValue));
    mp::maybe_null<Parent *> const maybe_null(data.get());
    auto opt = maybe_null.as_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<Parent *> parent = opt.value();
    REQUIRE(parent->value == parentValue);
  }
}

TEST_CASE("rvalue reference constructor", "[maybe_null]")
{
  SECTION("from child")
  {
    int *parentValue = new int(32);
    std::unique_ptr<Child> data(new Child(parentValue));
    mp::maybe_null<Parent *> const maybe_null(std::move(data.get()));
    auto opt = maybe_null.as_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<Parent *> parent = opt.value();
    REQUIRE(parent->value == parentValue);
  }
}

TEST_CASE("copy constructor", "[maybe_null]")
{
  SECTION("same type")
  {
    gsl::owner<Parent *> owner1{ new Parent() };
    *owner1->value = 42;
    mp::maybe_null<Parent *> const parent1{ owner1 };
    mp::maybe_null<Parent *> const parent2(parent1);
    auto opt = parent2.as_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<Parent *> not_null = opt.value();
    int actual = not_null->get_value();
    REQUIRE(actual == 42);
    delete owner1;
  }
  SECTION("from child")
  {
    gsl::owner<Child *> owner1{ new Child() };
    *owner1->value = 42;
    mp::maybe_null<Child *> const parent1{ owner1 };
    mp::maybe_null<Parent *> const parent2(parent1);
    auto opt = parent2.as_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<Parent *> not_null = opt.value();
    int actual = not_null->get_value();
    REQUIRE(actual == 42);
    delete owner1;
  }
  SECTION("nullptr")
  {
    mp::maybe_null<Child *> const parent1{ nullptr };
    mp::maybe_null<Parent *> const parent2(parent1);
    auto opt = parent2.as_not_null();
    REQUIRE(!opt.has_value());
  }
}

TEST_CASE("move constructor", "[maybe_null]")
{
  SECTION("same type")
  {
    gsl::owner<Parent *> owner1{ new Parent() };
    *owner1->value = 42;
    mp::maybe_null<Parent *> const parent1{ owner1 };
    mp::maybe_null<Parent *> const parent2(std::move(parent1));
    auto opt = parent2.as_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<Parent *> not_null = opt.value();
    int actual = not_null->get_value();
    REQUIRE(actual == 42);
    delete owner1;
  }
  SECTION("from child")
  {
    gsl::owner<Child *> owner1{ new Child() };
    *owner1->value = 42;
    mp::maybe_null<Child *> const parent1{ owner1 };
    mp::maybe_null<Parent *> const parent2(std::move(parent1));
    auto opt = parent2.as_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<Parent *> not_null = opt.value();
    int actual = not_null->get_value();
    REQUIRE(actual == 42);
    delete owner1;
  }
  SECTION("nullptr")
  {
    mp::maybe_null<Child *> const parent1{ nullptr };
    mp::maybe_null<Parent *> const parent2(std::move(parent1));
    auto opt = parent2.as_not_null();
    REQUIRE(!opt.has_value());
  }
}

TEST_CASE("assignment", "[maybe_null]")
{
  SECTION("same types")
  {
    int const data = 4;
    mp::maybe_null<int const *> const source{ &data };
    mp::maybe_null<int const *> destination;
    destination = source;
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
    mp::maybe_null<Parent *> destination;
    destination = source;
    auto opt = destination.as_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<Parent *> const &not_null = opt.value();
    int const actual = not_null->get_value();
    REQUIRE(*data == actual);
  }
}


TEST_CASE("move assignment", "[maybe_null]")
{
  SECTION("same types")
  {
    int const data = 4;
    mp::maybe_null<int const *> const source{ &data };
    mp::maybe_null<int const *> destination;
    destination = std::move(source);
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
    mp::maybe_null<Parent *> destination;
    destination = std::move(source);
    auto opt = destination.as_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<Parent *> const &not_null = opt.value();
    int const actual = not_null->get_value();
    REQUIRE(*data == actual);
  }
}

TEST_CASE("operator !", "[maybe_null]")
{
  SECTION("nullptr")
  {
    mp::maybe_null<int *> ptr;
    bool actual = !ptr;
    REQUIRE(actual);
  }
  SECTION("not nullptr")
  {
    int data = 4;
    mp::maybe_null<int *> ptr(&data);
    bool actual = !!ptr;
    REQUIRE(actual);
  }
}

TEST_CASE("make_maybe_null", "[maybe_null]")
{
  int const data = 4;
  mp::maybe_null<int const *> const maybe_null = mp::make_maybe_null(&data);
  auto opt = maybe_null.as_not_null();
  REQUIRE(opt.has_value());
  int const *actual = opt.value();
  REQUIRE(data == *actual);
}

TEST_CASE("stream operator", "[maybe_null]")
{
  int const data = 3;
  mp::maybe_null<int const *> const maybe_null = mp::make_maybe_null(&data);
  std::ostringstream actual_stream;
  actual_stream << maybe_null;
  std::ostringstream expected_stream;
  expected_stream << &data;
  std::string actual = actual_stream.str();
  std::string expected = expected_stream.str();
  REQUIRE(actual == expected);
}

TEST_CASE("comparison operators void*", "[maybe_null]")
{
  std::array<int, 3> data = { 2, 3, 4 };
  auto not_null1 = mp::make_maybe_null(&(data.at(1)));

  SECTION("operator ==")
  {
    REQUIRE(not_null1 == &data.at(1));
    REQUIRE(&data.at(1) == not_null1);
    REQUIRE_FALSE(not_null1 == &data.at(2));
    REQUIRE_FALSE(&data.at(2) == not_null1);
  }
  SECTION("operator !=")
  {
    REQUIRE_FALSE(not_null1 != &data.at(1));
    REQUIRE_FALSE(&data.at(1) != not_null1);
    REQUIRE(not_null1 != &data.at(2));
    REQUIRE(&data.at(2) != not_null1);
  }
  SECTION("operator <")
  {
    bool zero_lt = (&data.at(0) < not_null1);
    bool zero_gt = (&data.at(0) > not_null1);
    bool two_lt = (not_null1 < &data.at(2));
    bool two_gt = (not_null1 > &data.at(2));
    REQUIRE((zero_lt || zero_gt));
    REQUIRE((zero_lt ^ zero_gt));
    REQUIRE((two_lt || two_gt));
    REQUIRE(two_lt ^ two_gt);
    REQUIRE((zero_lt && two_lt));
    REQUIRE((zero_lt || two_lt));
  }
}


TEST_CASE("comparison operators maybe_null", "[maybe_null]")
{
  std::array<int, 3> data = { 2, 3, 4 };
  auto not_null0 = mp::make_maybe_null(&(data.at(0)));
  auto not_null1 = mp::make_maybe_null(&(data.at(1)));
  auto not_null1b = mp::make_maybe_null(&(data.at(1)));
  auto not_null2 = mp::make_maybe_null(&(data.at(2)));

  SECTION("operator ==")
  {
    REQUIRE(not_null1 == not_null1b);
    REQUIRE_FALSE(not_null1 == not_null2);
  }
  SECTION("operator !=")
  {
    REQUIRE_FALSE((not_null1 != not_null1b));
    bool ne = (not_null1 != not_null2);
    REQUIRE(ne);
  }
  SECTION("operator <")
  {
    bool zero_lt = (not_null0 < not_null1);
    bool zero_gt = (not_null0 > not_null1);
    bool two_lt = (not_null1 < not_null2);
    bool two_gt = (not_null1 > not_null2);
    REQUIRE((zero_lt || zero_gt));
    REQUIRE((zero_lt ^ zero_gt));
    REQUIRE((two_lt || two_gt));
    REQUIRE(two_lt ^ two_gt);
    REQUIRE((zero_lt && two_lt));
    REQUIRE((zero_lt || two_lt));
  }
}

// TEST_CASE("hashing", "[maybe_null]")
// {
//   int data = 3;
//   auto maybe_null = mp::make_maybe_null(&data);
//   auto actual = std::hash(maybe_null);
//   auto execpted = std::hash(&data);
//   REQUIRE(actual == expected);
// }


// NOLINTEND (cppcoreguidelines-avoid-magic-numbers)
