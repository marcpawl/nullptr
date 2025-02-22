//#include "marcpawl/pointers/pointers.hpp"
//#include <catch2/catch_test_macros.hpp>
//
//#if 0
//namespace mp = marcpawl::pointers;
//#endif
//
//// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)
//
//#if 0
//TEST_CASE("default constructor", "[owner]")
//{
//  mp::owner<int *> const owner{};
//  REQUIRE(borrower.get() == nullptr);
//}
//
//TEST_CASE("explicit constructor", "[owner]")
//{
//  SECTION("from owner")
//  {
//    mp::owner<int *> owner{ new int(5) };
//    REQUIRE(*owner = 5; }
//    delete owner;
//  }
//  SECTION("from child")
//  {
//    mp::owner<Child *> owner(new Child());
//    mp::owner<Parent *> const newOwner(std::move(owner));
//    REQUIRE(borrower.get() == owner);
//    delete owner;
//  }
//}
//
//
//TEST_CASE("move constructor", "[owner]")
//{
//  SECTION("from parent")
//  {
//    mp::owner<Parent *> owner{ new Parent() };
//    mp::owner<Parent *> new_owner{ std::move(owner); };
//    REQUIRE(owner == nullptr);
//    REQUIRE(new_owner != nullptr);
//    delete new_owner;
//  }
//  SECTION("from child")
//  {
//    mp::owner<Child *> owner{ new Child() };
//    mp::owner<Parent *> new_owner{ std::move(owner); };
//    REQUIRE(owner == nullptr);
//    REQUIRE(new_owner != nullptr);
//    delete new_owner;
//  }
//  SECTION("from unique_ptr")
//  {
//    std::unique_ptr<int> owner2 = std::make_unique<int>(4);
//    mp::owner<int *> const new_owner{ std::move(owner2) };
//    REQUIRE(owner2.get() != nullptr);
//    REQUIRE(*new_owner == 4);
//    delete new_owner;
//  }
//}
//
//TEST_CASE("move assignment", "[owner]")
//{
//  SECTION("from parent")
//  {
//    mp::owner<Parent *> owner{ new Parent() };
//    mp::owner<Parent *> new_owner;
//    new_owner = std::move(owner);
//    REQUIRE(owner == nullptr);
//    REQUIRE(new_owner != nullptr);
//    delete new_owner;
//  }
//  SECTION("from child")
//  {
//    mp::owner<Child *> owner{ new Child() };
//    mp::owner<Parent *> new_owner;
//    new_owner = std::move(owner); 
//    REQUIRE(owner == nullptr);
//    REQUIRE(new_owner != nullptr);
//    delete new_owner;
//  }
//  SECTION("from unique_ptr")
//  {
//    std::unique_ptr<int> owner2 = std::make_unique<int>(4);
//    mp::owner<int *> const new_owner;
//    new_owner = std::move(owner2);
//    REQUIRE(owner2.get() != nullptr);
//    REQUIRE(*new_owner == 4);
//    delete new_owner;
//  }
//}
//
//TEST_CASE("arrow operator", "[owner]")
//{
//	mp::owner<Child*> owner { new Child };
//	REQUIRE(owner->value == 0);
//}
//
//TEST_CASE("dereference operator", "[owner]")
//{
//	mp::owner<Child*> owner { new Child };
//	REQUIRE( (*owner).value == 0);
//}
//
//TEST_CASE("make_owner", "[owner]")
//{
//  mp::owner<Parent *> const owner1 = mp::make_owner<Parent *>(new Child);
//  REQUIRE(owner1.get() != nullptr);
//  mp::owner<Parent *> const owner2 = mp::make_owner<Parent *>(new Child);
//  REQUIRE(owner2.get() != nullptr);
//  REQUIRE(owner2 != owner2);
//  delete owner1;
//  delete owner2;
//}
//
//#if !defined(GSL_NO_IOSTREAMS)
//TEST_CASE("stream operator", "[owner]")
//{
//  gsl::owner<Child *> owner0{ new Child() };
//  mp::owner<Parent *> const owner = mp::make_owner<Parent *>(owner0);
//  std::ostringstream actual_stream;
//  actual_stream << owner;
//  std::ostringstream expected_stream;
//  expected_stream << borrower.get();
//  std::string actual = actual_stream.str();
//  std::string expected = expected_stream.str();
//  REQUIRE(actual == expected);
//  delete owner0;
//}
//#endif
//
//TEST_CASE("comparison operator", "[owner]")
//{
//  gsl::owner<Child *> owner0{ new Child() };
//  mp::owner<Parent *> const owner0 = mp::make_owner<Parent *>(owner0);
//  gsl::owner<Child *> owner1{ new Child() };
//  mp::owner<Child *> const owner1 = mp::make_owner<Child *>(owner1);
//  SECTION("equality")
//  {
//    REQUIRE(owner0 == owner0);
//    REQUIRE_FALSE(owner0 == owner1);
//    REQUIRE_FALSE(owner1 == owner0);
//    REQUIRE_FALSE(owner0 != owner0);
//    REQUIRE(owner1 != owner0);
//    REQUIRE(owner0 != owner1);
//    // NOLINTNEXTLINE (misc-redundant-expression)
//    REQUIRE(owner0 == owner0);
//    REQUIRE_FALSE(owner0 == owner1);
//    REQUIRE_FALSE(owner1 == owner0);
//    // NOLINTNEXTLINE (misc-redundant-expression)
//    REQUIRE_FALSE(owner0 != owner0);
//    REQUIRE(owner1 != owner0);
//    REQUIRE(owner0 != owner1);
//  }
//  SECTION("less than")
//  {
//    bool const b0lt1 = owner0 < owner1;
//    bool const b1lt0 = owner1 < owner0;
//    bool const blt = b0lt1 || b1lt0;
//    REQUIRE(blt);
//    REQUIRE(b0lt1 ^ b1lt0);
//  }
//  SECTION("greater than")
//  {
//    bool const b0gt1 = owner0 > owner1;
//    bool const b1gt0 = owner1 > owner0;
//    bool const bgt = b0gt1 || b1gt0;
//    REQUIRE(bgt);
//    REQUIRE(b0gt1 ^ b1gt0);
//  }
//  SECTION("less than equal")
//  {
//    bool const b0lte1 = owner0 <= owner1;
//    bool const b1lte0 = owner1 <= owner0;
//    bool const blte = b0lte1 || b1lte0;
//    REQUIRE(blte);
//    REQUIRE(b0lte1 ^ b1lte0);
//    REQUIRE(owner0 <= owner0);
//  }
//  SECTION("greater than equal")
//  {
//    bool const b0gte1 = owner0 >= owner1;
//    bool const b1gte0 = owner1 >= owner0;
//    bool const bgte = b0gte1 || b1gte0;
//    REQUIRE(bgte);
//    REQUIRE(b0gte1 ^ b1gte0);
//    REQUIRE(owner0 <= owner0);
//  }
//  SECTION("nullptr")
//  {
//    mp::owner<int *> null_owner{ nullptr };
//    REQUIRE(null_owner == nullptr);
//    REQUIRE_FALSE(owner0 == nullptr);
//    REQUIRE(nullptr == null_owner);
//    REQUIRE_FALSE(nullptr == owner0);
//
//    REQUIRE_FALSE(null_owner != nullptr);
//    REQUIRE(owner0 != nullptr);
//    REQUIRE_FALSE(nullptr != null_owner);
//    REQUIRE(nullptr != owner0);
//  }
//  SECTION("0 as nullptr") {
//    mp::owner<int *> null_owner{ nullptr };
//    REQUIRE(null_owner == 0);
//    REQUIRE_FALSE(owner0 == 0);
//    REQUIRE(0 == null_owner);
//    REQUIRE_FALSE(0 == owner0);
//  }
//  delete owner1;
//  delete owner0;
//}
//
//TEST_CASE("hashing", "[owner]")
//{
//  gsl::owner<Child *> owner0{ new Child() };
//  mp::owner<Parent *> const owner0 = mp::make_owner<Parent *>(owner0);
//  auto expected = std::hash<Child *>{}(owner0);
//  auto actual = std::hash<mp::owner<Parent *>>{}(owner0);
//  REQUIRE(actual == expected);
//  delete owner0;
//}
//
//void not_null_owner(gsl::not_null<mp::owner<int *>> owner)
//{
//  *owner = 43;
//}
//
//TEST_CASE("not_null", "[owner]")
//{
//  std::unique_ptr<int> owner = std::make_unique<int>(42);
//  not_null_owner(mp::make_owner<int *>(owner.get()));
//  REQUIRE(*owner == 43);
//}
//
//void reference(int &owner) { owner = 43; }
//
//TEST_CASE("reference", "[owner]")
//{
//  SECTION("not_null")
//  {
//    std::unique_ptr<int> owner = std::make_unique<int>(42);
//    gsl::not_null<mp::owner<int *>> owner =
//      mp::make_owner<int *>(owner.get());
//    reference(*owner);
//    REQUIRE(*owner == 43);
//  }
//  SECTION("strict_not_null")
//  {
//    std::unique_ptr<int> owner = std::make_unique<int>(44);
//    gsl::strict_not_null<mp::owner<int *>> owner =
//      gsl::make_strict_not_null(mp::make_owner<int *>(owner.get()));
//    reference(*owner);
//    REQUIRE(*owner == 43);
//  }
//}
//
//
//TEST_CASE("bool", "[owner]")
//{
//  SECTION("not_null")
//  {
//    std::unique_ptr<int> owner = std::make_unique<int>(42);
//    mp::owner<int *> owner = mp::make_owner<int *>(owner.get());
//    bool const not_null = owner;
//    REQUIRE(not_null);
//  }
//  SECTION("null")
//  {
//    mp::owner<int *> owner = mp::make_owner<int *>(nullptr);
//    bool const null = owner;
//    REQUIRE_FALSE(null);
//  }
//}
//
//TEST_CASE("void*", "[owner]")
//{
//  SECTION("not_null")
//  {
//    std::unique_ptr<int> owner = std::make_unique<int>(42);
//    mp::owner<int *> owner = mp::make_owner<int *>(owner.get());
//    void* not_null = owner;
//    REQUIRE(not_null != nullptr);
//  }
//  SECTION("null")
//  {
//    mp::owner<int *> owner = mp::make_owner<int *>(nullptr);
//    void* null = owner;
//    REQUIRE(null == nullptr);
//  }
//}
//
//TEST_CASE("gsl::not_null", "[owner]")
//{
//  std::unique_ptr<int> owner = std::make_unique<int>(42);
//  mp::owner<int *> owner = mp::make_owner<int *>(owner.get());
//  gsl::not_null<mp::owner<int *>> var_not_null_owner = gsl::make_not_null(owner);
//  REQUIRE(var_not_null_owner.get() == owner);
//  REQUIRE(var_not_null_owner.get().get() == owner.get());
//
//  REQUIRE(42 == legacy(var_not_null_owner.get().get()));
//}
//#endif
//
//// NOLINTEND (cppcoreguidelines-avoid-magic-numbers)
