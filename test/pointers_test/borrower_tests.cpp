#include <sstream>
#include <catch2/catch_test_macros.hpp>
#include "pointers.hpp"

namespace mp = marcpawl::pointers;

TEST_CASE("default constructor", "[borrower]")
{
  mp::borrower<int *> const borrower{};
  REQUIRE(borrower.get() == nullptr);
}


struct Parent
{
  int *value;
  Parent()
  {
    // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
    value = new int(0);
  }
  Parent(Parent &other) = delete;
  Parent(Parent &&other) noexcept
  {
    // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
    delete value;
    // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
    value = new int(other.get_value());
    // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
    delete other.value;
    other.value = nullptr;
  }
  virtual ~Parent() { delete value; }

  Parent &operator=(Parent &other) = delete;
  Parent &operator=(Parent &&other) noexcept
  {
    if (this == &other) { return *this; }
    // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
    delete value;
    // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
    value = new int(other.get_value());
    // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
    delete other.value;
    other.value = nullptr;
    return *this;
  }

  [[nodiscard]]
  int get_value() const
  {
    if (nullptr == value) { return 0; }
    return *value;
  }
};

struct Child : Parent
{
  int another_value = 0;

  Child() = default;
  Child(Child &other) = delete;
  Child(Child &&other) = default;
  ~Child() override = default;
  Child &operator=(Child &other) = delete;
  Child &operator=(Child &&other) = default;
};

TEST_CASE("delete child", "")
{
  // NOLINTNEXTLINE (hicpp-use-auto,modernize-use-auto)
  gsl::owner<Child *> owner(new Child());
  delete owner;
}

TEST_CASE("explicit constructor", "[borrower]")
{
  // NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)
  SECTION("from owner")
  {
    gsl::owner<int *> owner{ new int(5) };
    mp::borrower<int *> const borrower{ owner };
    REQUIRE(borrower.get() == owner);
    delete owner;
  }
  SECTION("from child")
  {
    gsl::owner<Child *> owner(new Child());
    mp::borrower<Parent *> const borrower(owner);
    REQUIRE(borrower.get() == owner);
    delete owner;
  }
  SECTION("from unique_ptr")
  {
    std::unique_ptr<int> owner2 = std::make_unique<int>(4);
    mp::borrower<int *> const borrower2{ owner2.get() };
    REQUIRE(borrower2.get() != nullptr);
    REQUIRE(*borrower2.get() == 4);
  }
  // NOLINTEND (cppcoreguidelines-avoid-magic-numbers)
}

TEST_CASE("copy constructor", "[borrower]")
{
  SECTION("from parent")
  {
    gsl::owner<Parent *> owner1{ new Parent() };
    mp::borrower<Parent *> const parent1{ owner1 };
    mp::borrower<Parent *> const parent2(parent1);
    REQUIRE(owner1 == parent1.get());
    REQUIRE(parent2.get() == parent1.get());
    delete owner1;
  }
  SECTION("from child")
  {
    // NOLINTNEXTLINE (hicpp-use-auto,modernize-use-auto)
    gsl::owner<Child *> owner2(new Child());
    mp::borrower<Child *> const child1(owner2);
    mp::borrower<Parent *> const parent3(child1);
    REQUIRE(owner2 == child1.get());
    REQUIRE(parent3.get() == child1.get());
    delete owner2;
  }
}

TEST_CASE("move constructor", "[borrower]")
{
  SECTION("from parent")
  {
    gsl::owner<Parent *> owner{ new Parent() };
    mp::borrower<Parent *> const parent1{ owner };
    // NOLINTNEXTLINE (hicpp-move-const-arg,performance-move-const-arg)
    // mp::borrower<Parent *> const parent2( std::move(parent1) );
    // REQUIRE(parent2.get() != nullptr);
    delete owner;
  }
  SECTION("from child")
  {
    // NOLINTNEXTLINE (hicpp-move-const-arg,performance-move-const-arg)
    gsl::owner<Child *> owner(new Child());
    mp::borrower<Child *> const child1(owner);
    // NOLINTNEXTLINE (hicpp-move-const-arg,performance-move-const-arg)
    // mp::borrower<Parent *> parent3( std::move(child1) );
    // REQUIRE(parent3.get() != nullptr);
    delete owner;
  }
}

TEST_CASE("assignment", "[borrower]")
{
  SECTION("from parent")
  {
    gsl::owner<Parent *> owner0{ new Parent() };
    gsl::owner<Parent *> owner1{ new Parent() };
    mp::borrower<Parent *>   parent1(owner0);
    mp::borrower<Parent *> const  parent2(owner1);
    parent1  = parent2;
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
    mp::borrower<Child *>  child1(owner2);
    mp::borrower<Child *> const child2(owner3);
    child1 = child2;
    REQUIRE(child1.get() == owner3);
    REQUIRE(child2.get() == owner3);
    delete owner3;
    delete owner2;
  }
}

TEST_CASE("move assignment", "[borrower]")
{
  SECTION("from parent")
  {
    gsl::owner<Parent *> owner0{ new Parent() };
    gsl::owner<Parent *> owner1{ new Parent() };
    mp::borrower<Parent *>   parent1(owner0);
    mp::borrower<Parent *> const  parent2(owner1);
    // NOLINTNEXTLINE (hicpp-move-const-arg,performance-move-const-arg)
    parent1  = std::move(parent2);
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
    mp::borrower<Child *>  child1(owner2);
    mp::borrower<Child *> const child2(owner3);
    // NOLINTNEXTLINE (hicpp-move-const-arg,performance-move-const-arg)
    child1 = std::move(child2);
    REQUIRE(child1.get() == owner3);
    delete owner3;
    delete owner2;
  }
}

TEST_CASE("arrow operator", "[borrower]")
{
    gsl::owner<Child *> owner0{ new Child() };
    mp::borrower< Parent*> const borrower = 
        mp::make_borrower<Parent*>(owner0);
    REQUIRE(borrower->value == owner0->value);
    delete owner0;
}

TEST_CASE("dereference operator", "[borrower]")
{
    gsl::owner<Child *> owner0{ new Child() };
    mp::borrower< Parent*> const borrower = 
        mp::make_borrower<Parent*>(owner0);
    REQUIRE((*borrower).value == owner0->value);
    delete owner0;
}

TEST_CASE("make_borrower", "[borrower]")
{
    gsl::owner<Child *> owner0{ new Child() };
    mp::borrower< Parent*> const borrower = 
        mp::make_borrower<Parent*>(owner0);
    REQUIRE(borrower.get() == owner0);
    delete owner0;
}

#if !defined(GSL_NO_IOSTREAMS)
TEST_CASE("stream operator", "[borrower]")
{
    gsl::owner<Child *> owner0{ new Child() };
    mp::borrower< Parent*> const borrower = 
        mp::make_borrower<Parent*>(owner0);
    std::ostringstream actual_stream;
    actual_stream << borrower;
    std::ostringstream expected_stream;
    expected_stream << borrower.get();
    std::string actual =actual_stream.str();
    std::string expected = expected_stream.str();
    REQUIRE(actual == expected);
    delete owner0;
}
#endif

