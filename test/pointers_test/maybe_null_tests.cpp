#include "hierarchy.hpp"
#include "marcpawl/pointers/ptr.hpp"
#include <catch2/catch_test_macros.hpp>

#include <gsl/gsl>

namespace mp = marcpawl::pointers;

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)

TEST_CASE("maybe_null default constructor", "[maybe_null]")
{
  mp::maybe_null<int *> const maybe_null;
  auto opt = maybe_null.as_optional_not_null();
  REQUIRE(!opt.has_value());
}

TEST_CASE("explicit nullptr", "[maybe_null]")
{
  mp::maybe_null<int *> const maybe_null{ nullptr };
  auto opt = maybe_null.as_optional_not_null();
  REQUIRE(!opt.has_value());
}

TEST_CASE("explicit constructor", "[maybe_null]")
{
  SECTION("from typed nullptr")
  {
    int *data = nullptr;
    mp::maybe_null<int *> const maybe_null(data);
    auto opt = maybe_null.as_optional_not_null();
    REQUIRE(!opt.has_value());
  }
  SECTION("from non-null")
  {
    int data = 32;
    mp::maybe_null<int *> maybe_null(&data);
    auto opt = maybe_null.as_optional_not_null();
    REQUIRE(opt.has_value());
    mp::strict_not_null<int *> &not_null = opt.value();
    REQUIRE(data == *not_null);
  }
  SECTION("from child")
  {
    int *parentValue = new int(32);
    std::unique_ptr<Child> data(new Child(parentValue));
    mp::maybe_null<Parent *> const maybe_null(data.get());
    auto opt = maybe_null.as_optional_not_null();
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
    auto opt = maybe_null.as_optional_not_null();
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
    auto opt = parent2.as_optional_not_null();
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
    auto opt = parent2.as_optional_not_null();
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
    auto opt = parent2.as_optional_not_null();
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
    auto opt = parent2.as_optional_not_null();
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
    auto opt = parent2.as_optional_not_null();
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
    auto opt = parent2.as_optional_not_null();
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
    mp::maybe_null<Parent *> destination;
    destination = source;
    auto opt = destination.as_optional_not_null();
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
    mp::maybe_null<Parent *> destination;
    destination = std::move(source);
    auto opt = destination.as_optional_not_null();
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
  auto opt = maybe_null.as_optional_not_null();
  REQUIRE(opt.has_value());
  mp::strict_not_null<int const *> actual = opt.value();
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
  int *data0 = &(data.at(0));
  int *data1 = &(data.at(1));
  int *data2 = &(data.at(2));
  mp::maybe_null<int *> const not_null1 = mp::make_maybe_null(data1);

  SECTION("operator ==")
  {
    bool eq = (not_null1 == data1);
    REQUIRE(eq);
    REQUIRE((data1 == not_null1));
    REQUIRE_FALSE((not_null1 == data2));
    REQUIRE_FALSE((data2 == not_null1));
  }
  SECTION("operator !=")
  {
    REQUIRE_FALSE((not_null1 != data1));
    REQUIRE_FALSE((data1 != not_null1));
    REQUIRE((not_null1 != data2));
    REQUIRE((data2 != not_null1));
  }
  SECTION("operator <")
  {
    bool zero_lt = (data0 < not_null1);
    bool zero_gt = (data0 > not_null1);
    bool two_lt = (not_null1 < data2);
    bool two_gt = (not_null1 > data2);
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
  int *data0 = &(data.at(0));
  int *data1 = &(data.at(1));
  int *data2 = &(data.at(2));
  mp::maybe_null<int *> not_null0 = mp::make_maybe_null(data0);
  mp::maybe_null<int *> const not_null1 = mp::make_maybe_null(data1);
  mp::maybe_null<int *> const not_null1b = mp::make_maybe_null(data1);
  auto not_null2 = mp::make_maybe_null(data2);

  SECTION("operator ==")
  {
    bool eq = (not_null1 == not_null1b);
    REQUIRE(eq);
    REQUIRE_FALSE((not_null1 == not_null2));
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


TEST_CASE("as_variant_not_null", "[maybe_null]")
{
  SECTION("from typed nullptr")
  {
    int *data = nullptr;
    mp::maybe_null<int *> const maybe_null(data);
    mp::maybe_null<int *>::variant_not_null variant =
      maybe_null.as_variant_not_null();
    bool is_null = std::holds_alternative<std::nullptr_t>(variant);
    REQUIRE(is_null);
  }
  SECTION("from non-null")
  {
    int data = 32;
    mp::maybe_null<int *> maybe_null(&data);
    auto variant = maybe_null.as_variant_not_null();
    auto has_data = std::holds_alternative<mp::strict_not_null<int *>>(variant);
    REQUIRE(has_data);
    mp::strict_not_null<int *> not_null =
      std::get<mp::strict_not_null<int *>>(variant);
    REQUIRE(data == *not_null);
  }
}

TEST_CASE("constraints", "[maybe_null]")
{
  SECTION("not_null_handler")
  {
    auto handle_data = [](auto &&) {};
    mp::not_null_handler<int const *> auto func = handle_data;

    int const data = 4;
    mp::maybe_null<int const *> const source{ &data };
    auto opt = source.as_optional_not_null();
    mp::strict_not_null<int const *> const &not_null = opt.value();

    func(not_null);
  }
}

TEST_CASE("visit", "[maybe_null]")
{
  SECTION("from typed nullptr")
  {
    int *data = nullptr;
    mp::maybe_null<int *> const maybe_null(data);
    bool is_null = false;
    auto handle_null = [&is_null](std::nullptr_t) { is_null = true; };
    auto handle_data = [&is_null](auto &&) { is_null = false; };
    maybe_null.visit(handle_null, handle_data);
    REQUIRE(is_null);
  }
  SECTION("from non-null")
  {
    int data = 32;
    mp::maybe_null<int *> maybe_null(&data);
    auto handle_null = [](std::nullptr_t) noexcept {};
    auto handle_data = [](mp::strict_not_null<int *> &&not_null) noexcept {
      (*not_null) += 1;
    };
    maybe_null.visit(handle_null, handle_data);
    REQUIRE(data == 33);
  }
  SECTION("noexcept nobody")
  {
    int *data = nullptr;
    mp::maybe_null<int *> const maybe_null(data);
    auto handle_null = [](std::nullptr_t) {};
    auto handle_data = [](auto &&) {};
    bool is_noexcept = noexcept(maybe_null.visit(handle_null, handle_data));
    REQUIRE_FALSE(is_noexcept);
  }
  SECTION("noexcept handle null")
  {
    int *data = nullptr;
    mp::maybe_null<int *> const maybe_null(data);
    auto handle_null = [](std::nullptr_t) noexcept {};
    REQUIRE(noexcept(handle_null(nullptr)));
    auto handle_data = [](auto &&) {};
    bool is_noexcept = noexcept(maybe_null.visit(handle_null, handle_data));
    REQUIRE_FALSE(is_noexcept);
  }
  SECTION("noexcept handle data")
  {
    int data = 3;
    mp::maybe_null<int *> const maybe_null(&data);
    auto handle_null = [](std::nullptr_t) {};
    auto handle_data = [](auto &&) noexcept {};
    auto opt = maybe_null.as_optional_not_null();
    mp::strict_not_null<int *> not_null = opt.value();
    REQUIRE(noexcept(handle_data(not_null)));
    bool is_noexcept = noexcept(maybe_null.visit(handle_null, handle_data));
    REQUIRE_FALSE(is_noexcept);
  }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
TEST_CASE("get", "[maybe_null]")
{
  SECTION("nullptr")
  {
    int *data = nullptr;
    mp::maybe_null<int *> sut = mp::make_maybe_null(data);
    int *actual = sut.get();
    REQUIRE(actual == nullptr);
  }
  SECTION("not nullptr")
  {
    int const data = 5;
    mp::maybe_null<int const *> sut = mp::make_maybe_null(&data);
    int const *actual = sut.get();
    REQUIRE(*actual == data);
  }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
TEST_CASE("arrow operator", "[maybe_null]")
{
  SECTION("nullptr")
  {
    Parent *data = nullptr;
    mp::maybe_null<Parent *> sut = mp::make_maybe_null(data);
    bool exceptionThrown = false;
    try {
      if (sut->value == 0) { abort(); }
    } catch (mp::nullptr_exception &) {
      exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
  }
  SECTION("not nullptr")
  {
    Parent data;
    mp::maybe_null<Parent *> sut = mp::make_maybe_null(&data);
    auto actual = sut->value;
    REQUIRE(data.value == actual);
  }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
TEST_CASE("star operator", "[maybe_null]")
{
  SECTION("nullptr")
  {
    int *data = nullptr;
    mp::maybe_null<int *> sut = mp::make_maybe_null(data);
    bool exceptionThrown = false;
    try {
      if (*sut) { abort(); }
    } catch (mp::nullptr_exception &) {
      exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
  }
  SECTION("not nullptr")
  {
    int data = 32;
    mp::maybe_null<int *> sut = mp::make_maybe_null(&data);
    auto actual = *sut;
    REQUIRE(data == actual);
  }
}
#pragma clang diagnostic pop

// TEST_CASE("hashing", "[maybe_null]")
// {
//   int data = 3;
//   auto maybe_null = mp::make_maybe_null(&data);
//   auto actual = std::hash(maybe_null);
//   auto execpted = std::hash(&data);
//   REQUIRE(actual == expected);
// }


// NOLINTEND (cppcoreguidelines-avoid-magic-numbers)
