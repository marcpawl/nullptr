#include "hierarchy.hpp"
#include "marcpawl/pointers/ptr.hpp"
#include <catch2/catch_test_macros.hpp>
#include <sstream>

#include <gsl/gsl>

namespace mp = marcpawl::pointers;

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)

TEST_CASE("borrower default constructor", "[borrower]")
{
  mp::borrower<int *> const borrower{};
  REQUIRE((borrower.get() == nullptr));
}

TEST_CASE("explicit constructor", "[borrower]")
{
  SECTION("from nullptr")
  {
    mp::borrower<int *> const borrower{ nullptr };
    bool actual = borrower.get() == nullptr;
    REQUIRE(actual);
  }
  SECTION("from owner")
  {
    gsl::owner<int *> owner{ new int(5) };
    mp::borrower<int *> const borrower = mp::make_borrower(owner);
    REQUIRE((borrower.get() == owner));
    delete owner;
  }
  SECTION("from child")
  {
    gsl::owner<Child *> owner(new Child());
    mp::borrower<Parent *> const borrower = mp::make_borrower(owner);
    REQUIRE((borrower.get() == owner));
    delete owner;
  }
  SECTION("from unique_ptr")
  {
    std::unique_ptr<int> owner2 = std::make_unique<int>(4);
    mp::borrower<int *> const borrower2{ owner2 };
    REQUIRE((borrower2.get() != nullptr));
    REQUIRE((*borrower2.get() == 4));
  }
  SECTION("from shared_ptr")
  {
    std::shared_ptr<int> owner2 = std::make_shared<int>(4);
    mp::borrower<int *> const borrower2{ owner2 };
    REQUIRE((borrower2.get() != nullptr));
    REQUIRE((*borrower2.get() == 4));
  }
  SECTION("from non-null")
  {
    int data = 32;
    mp::strict_not_null<int *> not_null{ &data };
    REQUIRE((32 == *not_null));
    REQUIRE_FALSE((not_null == nullptr));

    mp::borrower<mp::strict_not_null<int *>> source =
      mp::make_borrower(not_null);
    REQUIRE((data == *source));
  }
}

TEST_CASE("copy constructor", "[borrower]")
{
  SECTION("from parent")
  {
    gsl::owner<Parent *> owner1{ new Parent() };
    mp::borrower<Parent *> const parent1 = mp::make_borrower(owner1);
    mp::borrower<Parent *> const parent2(parent1);
    REQUIRE((owner1 == parent1.get()));
    REQUIRE((parent2.get() == parent1.get()));
    delete owner1;
  }
  SECTION("from child")
  {
    // NOLINTNEXTLINE (hicpp-use-auto,modernize-use-auto)
    gsl::owner<Child *> owner2(new Child());
    mp::borrower<Child *> const child1 = mp::make_borrower(owner2);
    mp::borrower<Parent *> const parent3(child1);
    REQUIRE((owner2 == child1.get()));
    REQUIRE((parent3.get() == child1.get()));
    delete owner2;
  }
  SECTION("from not null")
  {
    int data = 32;
    mp::strict_not_null<int *> not_null{ &data };
    mp::borrower<mp::strict_not_null<int *>> source =
      mp::make_borrower(not_null);
    mp::borrower<mp::strict_not_null<int *>> destination{ source };
    REQUIRE((data == *source));
    REQUIRE((data == *destination));
  }
}

TEST_CASE("move constructor", "[borrower]")
{
  SECTION("from parent")
  {
    gsl::owner<Parent *> owner{ new Parent() };
    mp::borrower<Parent *> const parent1 = mp::make_borrower(owner);
    // NOLINTNEXTLINE (hicpp-move-const-arg,performance-move-const-arg)
    mp::borrower<Parent *> const parent2(std::move(parent1));
    REQUIRE((parent2.get() != nullptr));
    delete owner;
  }
  SECTION("from child")
  {
    // NOLINTNEXTLINE (hicpp-move-const-arg,performance-move-const-arg)
    gsl::owner<Child *> owner(new Child());
    mp::borrower<Child *> const child1 = mp::make_borrower(owner);
    // NOLINTNEXTLINE (hicpp-move-const-arg,performance-move-const-arg)
    mp::borrower<Parent *> parent3(std::move(child1));
    REQUIRE((parent3.get() != nullptr));
    delete owner;
  }
}

TEST_CASE("assignment", "[borrower]")
{
  SECTION("from parent")
  {
    gsl::owner<Parent *> owner0{ new Parent() };
    gsl::owner<Parent *> owner1{ new Parent() };
    mp::borrower<Parent *> parent1 = mp::make_borrower(owner0);
    mp::borrower<Parent *> const parent2 = mp::make_borrower(owner1);
    REQUIRE(parent1.get() == owner0);
    parent1 = parent2;
    REQUIRE((owner1 == parent1.get()));
    REQUIRE((owner1 == parent2.get()));
    delete owner1;
    delete owner0;
  }
  SECTION("from child")
  {
    // NOLINTNEXTLINE (hicpp-use-auto,modernize-use-auto)
    gsl::owner<Child *> owner2(new Child());
    // NOLINTNEXTLINE (hicpp-use-auto,modernize-use-auto)
    gsl::owner<Child *> const owner3(new Child());
    mp::borrower<Child *> child1 = mp::make_borrower(owner2);
    mp::borrower<Child *> const child2 = mp::make_borrower(owner3);
    REQUIRE((child1.get() == owner2));
    child1 = child2;
    REQUIRE((child1.get() == owner3));
    REQUIRE((child2.get() == owner3));
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
    mp::borrower<Parent *> parent1 = mp::make_borrower(owner0);
    mp::borrower<Parent *> const parent2 = mp::make_borrower(owner1);
    REQUIRE(parent1.get() == owner0);
    // NOLINTNEXTLINE (hicpp-move-const-arg,performance-move-const-arg)
    parent1 = std::move(parent2);
    REQUIRE((owner1 == parent1.get()));
    delete owner1;
    delete owner0;
  }
  SECTION("from child")
  {
    // NOLINTNEXTLINE (hicpp-use-auto,modernize-use-auto)
    gsl::owner<Child *> owner2(new Child());
    // NOLINTNEXTLINE (hicpp-use-auto,modernize-use-auto)
    gsl::owner<Child *> const owner3(new Child());
    mp::borrower<Child *> child1 = mp::make_borrower(owner2);
    mp::borrower<Child *> const child2 = mp::make_borrower(owner3);
    REQUIRE(child1.get() == owner2);
    // NOLINTNEXTLINE (hicpp-move-const-arg,performance-move-const-arg)
    child1 = std::move(child2);
    REQUIRE((child1.get() == owner3));
    delete owner3;
    delete owner2;
  }
}

TEST_CASE("arrow operator", "[borrower]")
{
  gsl::owner<Child *> owner0{ new Child() };
  mp::borrower<Parent *> const borrower = mp::make_borrower<Parent *>(owner0);
  REQUIRE((borrower->value == owner0->value));
  delete owner0;
}

TEST_CASE("dereference operator", "[borrower]")
{
  gsl::owner<Child *> owner0{ new Child() };
  mp::borrower<Parent *> const borrower = mp::make_borrower<Parent *>(owner0);
  REQUIRE(((*borrower).value == owner0->value));
  delete owner0;
}

TEST_CASE("make_borrower", "[borrower]")
{
  SECTION("From pointer")
  {
    gsl::owner<Child *> owner0{ new Child() };
    mp::borrower<Parent *> const borrower1 =
      mp::make_borrower<Parent *>(owner0);
    REQUIRE((borrower1.get() == owner0));
    mp::borrower<Parent *> const borrower2 =
      mp::make_borrower<Parent *>(owner0);
    REQUIRE((borrower2.get() == owner0));
    delete owner0;
  }
  SECTION("from unique_ptr")
  {
    std::unique_ptr<int> owner2 = std::make_unique<int>(4);
    mp::borrower<int *> const borrower2 = mp::make_borrower(owner2);
    REQUIRE((borrower2.get() != nullptr));
    REQUIRE((*borrower2.get() == 4));
  }
  SECTION("from shared_ptr")
  {
    std::shared_ptr<int> owner2 = std::make_shared<int>(4);
    mp::borrower<int *> const borrower2 = mp::make_borrower(owner2);
    REQUIRE((borrower2.get() != nullptr));
    REQUIRE((*borrower2.get() == 4));
  }
}

#if !defined(GSL_NO_IOSTREAMS)
TEST_CASE("stream operator", "[borrower]")
{
  gsl::owner<Child *> owner0{ new Child() };
  mp::borrower<Parent *> const borrower = mp::make_borrower<Parent *>(owner0);
  std::ostringstream actual_stream;
  actual_stream << borrower;
  std::ostringstream expected_stream;
  expected_stream << borrower.get();
  std::string actual = actual_stream.str();
  std::string expected = expected_stream.str();
  REQUIRE((actual == expected));
  delete owner0;
}
#endif

TEST_CASE("comparison operator", "[borrower]")
{
  gsl::owner<Child *> owner0{ new Child() };
  mp::borrower<Parent *> const borrower0 = mp::make_borrower<Parent *>(owner0);
  gsl::owner<Child *> owner1{ new Child() };
  mp::borrower<Child *> const borrower1 = mp::make_borrower<Child *>(owner1);

  REQUIRE((owner0 == owner0));
  REQUIRE((owner0 != owner1));

  SECTION("equality")
  {
    bool const eq00 = (borrower0 == borrower0);
    REQUIRE(eq00);
    bool const eq01 = (borrower0 == borrower1);
    REQUIRE_FALSE(eq01);
    bool const eq10 = (borrower1 == borrower0);
    REQUIRE_FALSE(eq10);
    bool const ne00 = (borrower0 != borrower0);
    REQUIRE_FALSE(ne00);
    bool const ne10 = (borrower1 != borrower0);
    REQUIRE(ne10);
    bool const ne01 = (borrower0 != borrower1);
    REQUIRE(ne01);
  }
  SECTION("less than")
  {
    bool const b0lt1 = borrower0 < borrower1;
    bool const b1lt0 = borrower1 < borrower0;
    bool const blt = b0lt1 || b1lt0;
    REQUIRE(blt);
    REQUIRE(b0lt1 ^ b1lt0);
  }
  SECTION("greater than")
  {
    bool const b0gt1 = borrower0 > borrower1;
    bool const b1gt0 = borrower1 > borrower0;
    bool const bgt = b0gt1 || b1gt0;
    REQUIRE(bgt);
    REQUIRE(b0gt1 ^ b1gt0);
  }
  SECTION("less than equal")
  {
    bool const b0lte1 = borrower0 <= borrower1;
    bool const b1lte0 = borrower1 <= borrower0;
    bool const blte = b0lte1 || b1lte0;
    REQUIRE(blte);
    REQUIRE(b0lte1 ^ b1lte0);
    REQUIRE((borrower0 <= borrower0));
  }
  SECTION("greater than equal")
  {
    bool const b0gte1 = borrower0 >= borrower1;
    bool const b1gte0 = borrower1 >= borrower0;
    bool const bgte = b0gte1 || b1gte0;
    REQUIRE(bgte);
    REQUIRE(b0gte1 ^ b1gte0);
    REQUIRE((borrower0 <= borrower0));
  }
  SECTION("nullptr")
  {
    mp::borrower<int *> null_borrower = mp::make_borrower<int *>(nullptr);
    REQUIRE(null_borrower == nullptr);
    REQUIRE_FALSE(borrower0 == nullptr);
    REQUIRE(nullptr == null_borrower);
    REQUIRE_FALSE(nullptr == borrower0);

    REQUIRE_FALSE(null_borrower != nullptr);
    REQUIRE(borrower0 != nullptr);
    REQUIRE_FALSE(nullptr != null_borrower);
    REQUIRE(nullptr != borrower0);
  }
  SECTION("0 as nullptr")
  {
    // mp::borrower<int *> null_borrower = mp::make_borrower<int*>(nullptr );
    // TODO REQUIRE(null_borrower == 0);
    // TODO REQUIRE_FALSE(borrower0 == 0);
    // TODO REQUIRE(0 == null_borrower);
    // TODO REQUIRE_FALSE(0 == borrower0);
  }
  delete owner1;
  delete owner0;
}

TEST_CASE("nullable not_null comparison operator", "[borrower]")
{
  int const a = 21;
  int const b = 22;
  mp::strict_not_null<int const *> aa = mp::strict_not_null(&a);
  mp::strict_not_null<int const *> bb = mp::strict_not_null(&b);
  mp::borrower<int const *> nullable_a = mp::make_borrower<int const *>(&a);
  mp::borrower<mp::strict_not_null<int const *>> not_null_a =
    mp::make_borrower<mp::strict_not_null<int const *>>(aa);
  mp::borrower<mp::strict_not_null<int const *>> not_null_b =
    mp::make_borrower<mp::strict_not_null<int const *>>(bb);
  SECTION("operator==")
  {
    bool eq = (nullable_a == not_null_a);
    REQUIRE(eq);
    bool const eqaa = (not_null_a == nullable_a);
    REQUIRE(eqaa);
    REQUIRE((nullable_a <= not_null_a));
    REQUIRE((not_null_a >= nullable_a));
  }
  SECTION("operator!=")
  {
    REQUIRE((nullable_a != not_null_b));
    REQUIRE((not_null_b != nullable_a));
  }
  SECTION("<=")
  {
    bool nullalble_a_le_not_null_b = (nullable_a <= not_null_b);
    bool not_null_b_le_nullable_a = (not_null_b <= nullable_a);
    bool not_null_a_eq_not_null_b = (not_null_a == not_null_b);
    REQUIRE_FALSE(not_null_a_eq_not_null_b);
    REQUIRE((nullalble_a_le_not_null_b || not_null_b_le_nullable_a));
    REQUIRE((nullalble_a_le_not_null_b ^ not_null_b_le_nullable_a));
  }
  SECTION("<")
  {
    bool not_null_b_lt_nullable_a = (not_null_b < nullable_a);
    bool nullable_a_lt_not_null_b = (nullable_a < not_null_b);
    REQUIRE((not_null_b_lt_nullable_a || nullable_a_lt_not_null_b));
    REQUIRE((not_null_b_lt_nullable_a ^ nullable_a_lt_not_null_b));
  }
  SECTION(">")
  {
    bool not_null_b_gt_nullable_a = (not_null_b > nullable_a);
    bool nullable_a_gt_not_null_b = (nullable_a > not_null_b);
    REQUIRE((not_null_b_gt_nullable_a || nullable_a_gt_not_null_b));
    REQUIRE((not_null_b_gt_nullable_a ^ nullable_a_gt_not_null_b));
  }
  SECTION(">=")
  {
    bool nullalble_a_ge_not_null_b = (nullable_a >= not_null_b);
    bool not_null_b_ge_nullable_a = (not_null_b >= nullable_a);
    bool not_null_a_eq_not_null_b = (not_null_a == not_null_b);
    REQUIRE_FALSE(not_null_a_eq_not_null_b);
    REQUIRE((nullalble_a_ge_not_null_b || not_null_b_ge_nullable_a));
    REQUIRE((nullalble_a_ge_not_null_b ^ not_null_b_ge_nullable_a));
  }
}

void not_null_borrower(gsl::not_null<mp::borrower<int *>> borrower)
{
  *borrower = 43;
}

TEST_CASE("not_null", "[borrower]")
{
  std::unique_ptr<int> owner = std::make_unique<int>(42);
  not_null_borrower(mp::make_borrower<int *>(owner.get()));
  REQUIRE(*owner == 43);
}

void reference(int &borrower) { borrower = 43; }

TEST_CASE("reference", "[borrower]")
{
  SECTION("not_null")
  {
    std::unique_ptr<int> owner = std::make_unique<int>(42);
    auto borrower = gsl::make_not_null(mp::make_borrower<int *>(owner.get()));
    reference(*borrower);
    REQUIRE(*owner == 43);
  }
  SECTION("strict_not_null")
  {
    std::unique_ptr<int> owner = std::make_unique<int>(44);
    auto borrower =
      gsl::make_strict_not_null(mp::make_borrower<int *>(owner.get()));
    reference(*borrower);
    REQUIRE(*owner == 43);
  }
}


TEST_CASE("void*", "[borrower]")
{
  SECTION("not_null")
  {
    std::unique_ptr<int> owner = std::make_unique<int>(42);
    mp::borrower<int *> borrower = mp::make_borrower<int *>(owner.get());
    void *not_null = borrower.get();
    REQUIRE(not_null != nullptr);
  }
  SECTION("null")
  {
    mp::borrower<int *> borrower = mp::make_borrower<int *>(nullptr);
    void *null = borrower.get();
    REQUIRE(null == nullptr);
  }
}

TEST_CASE("gsl::not_null", "[borrower]")
{
  std::unique_ptr<int> owner = std::make_unique<int>(42);
  mp::borrower<int *> borrower = mp::make_borrower<int *>(owner.get());
  auto var_not_null_borrower = gsl::make_not_null(borrower);
  REQUIRE((var_not_null_borrower.get() == borrower));
  REQUIRE(var_not_null_borrower.get().get() == owner.get());

  REQUIRE(42 == legacy(var_not_null_borrower.get().get()));
}

// NOLINTEND (cppcoreguidelines-avoid-magic-numbers)
