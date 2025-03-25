#include <cassert>
#include <memory>

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

TEST_CASE("get", "[not_null]")
{
  int data = 32;
  mp::maybe_null<int *> maybe_null(&data);
  auto opt = maybe_null.as_optional_not_null();
  REQUIRE(opt.has_value());
  mp::strict_not_null<int *> const &not_null = opt.value();
  REQUIRE(data == *not_null);
  doubled(not_null.get());
  REQUIRE(64 == *not_null);
}

TEST_CASE("operator==", "[not_null]")
{
  SECTION("strict_not_null")
  {
    int data1 = 32;
    mp::maybe_null<int *> maybe_null1(&data1);
    auto opt1 = maybe_null1.as_optional_not_null();
    REQUIRE(opt1.has_value());
    mp::strict_not_null<int *> const &not_null1 = opt1.value();
    bool eq = (not_null1 == not_null1);
    REQUIRE(eq);
    int data2 = 33;
    mp::maybe_null<int *> maybe_null2(&data2);
    auto opt2 = maybe_null2.as_optional_not_null();
    REQUIRE(opt2.has_value());
    mp::strict_not_null<int *> const &not_null2 = opt2.value();
    bool eq2 = (not_null1 == not_null2);
    REQUIRE_FALSE(eq2);
  }
  SECTION("nullptr")
  {
    int data = 32;
    mp::maybe_null<int *> maybe_null(&data);
    auto opt = maybe_null.as_optional_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<int *> const &not_null = opt.value();
    bool eq = (not_null == nullptr);
    REQUIRE_FALSE(eq);
    bool eqr = (nullptr == not_null);
    REQUIRE_FALSE(eqr);
  }
  SECTION("ptr")
  {
    int data = 32;
    mp::maybe_null<int *> maybe_null(&data);
    auto opt = maybe_null.as_optional_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<int *> const &not_null = opt.value();
    mp::details::not_nullptr auto pdata = &data;
    bool eq = (not_null == pdata);
    REQUIRE(eq);
    bool eqr = (&data == not_null);
    REQUIRE(eqr);
  }
}

TEST_CASE("operator!=", "[not_null]")
{
  SECTION("nullptr")
  {
    int data = 32;
    mp::maybe_null<int *> maybe_null(&data);
    auto opt = maybe_null.as_optional_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<int *> const &not_null = opt.value();
    bool ne = (not_null != nullptr);
    REQUIRE(ne);
    bool ner = (nullptr != not_null);
    REQUIRE(ner);
  }
}


TEST_CASE("comparison operators", "[not_null]")
{
  SECTION("strict_not_null")
  {
    int data1 = 32;
    mp::maybe_null<int *> maybe_null1(&data1);
    auto opt1 = maybe_null1.as_optional_not_null();
    REQUIRE(opt1.has_value());
    mp::strict_not_null<int *> const &not_null1 = opt1.value();
    REQUIRE((not_null1 == not_null1));
    bool le11 = (not_null1 <= not_null1);
    REQUIRE(le11);
    bool ge11 = (not_null1 >= not_null1);
    REQUIRE(ge11);
    int data2 = 33;
    mp::maybe_null<int *> maybe_null2(&data2);
    auto opt2 = maybe_null2.as_optional_not_null();
    REQUIRE(opt2.has_value());
    mp::strict_not_null<int *> const &not_null2 = opt2.value();
    bool eq2 = (not_null1 == not_null2);
    REQUIRE_FALSE(eq2);
    bool le12 = (not_null1 < not_null2);
    bool ge12 = (not_null1 > not_null2);
    REQUIRE((le12 ^ ge12));
  }
}

TEST_CASE("copy constructor", "[not_null]")
{
  SECTION("same types")
  {
    int const data = 4;
    mp::maybe_null<int const *> const source{ &data };
    mp::maybe_null<int const *> const destination{ source };
    auto opt = destination.as_optional_not_null();
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
    auto opt = destination.as_optional_not_null();
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
    auto opt = destination.as_optional_not_null();
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
    auto opt = destination.as_optional_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<Parent *> const &not_null = opt.value();
    int const actual = not_null->get_value();
    REQUIRE(*data == actual);
  }
}

TEST_CASE("unique_ptr", "[not_null]")
{
  SECTION("not null")
  {
    std::unique_ptr<int> data = std::make_unique<int>(4);
    mp::maybe_null<std::unique_ptr<int>> source{ std::move(data) };
    auto opt = std::move(source).as_optional_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<std::unique_ptr<int>> const &not_null = opt.value();
    int const actual = *not_null;
    REQUIRE(4 == actual);
  }
#ifdef TODO
  SECTION("constructing nullptr")
  {
    std::unique_ptr<int> data = std::make_unique<int>();
    mp::maybe_null<std::unique_ptr<int>> const source{ std::move(data) };
    //    auto opt = source.as_optional_not_null();
    //  REQUIRE(opt.has_value());
    // mp::strict_not_null<std::unique_ptr<int>> const &not_null = opt.value();
  }
#ifdef TODO
  // MOVE TO COMPILE TIME ERROR
  SECTION("assigning nullptr")
  {
    std::unique_ptr<int> data = std::make_unique<int>(4);
    mp::maybe_null<std::unique_ptr<int>> const source{ data };
    auto opt = source.as_optional_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<std::unique_ptr<int>> const &not_null = opt.value();
    not_null = nullptr;
  }
#endif
#endif
}


// NOLINTEND (cppcoreguidelines-avoid-magic-numbers)
