#include "doublelist.hpp"
#include "pointers.hpp"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

namespace mp = marcpawl::pointers;

// NOLINTBEGIN(hicpp-named-parameter,readability-named-parameter)
struct Vocal
{
  Vocal() { std::clog << "default constructor\n"; }

  // NOLINTNEXTLINE(hicpp-named-parameter,readability-named-parameter)
  Vocal(Vocal const &) { std::clog << "copy constructor\n"; }

  // NOLINTNEXTLINE(hicpp-named-parameter,readability-named-parameter)
  Vocal(Vocal &&) noexcept { std::clog << "move constructor\n"; }

  ~Vocal() { std::clog << "destructor\n"; }

  Vocal &operator=(Vocal const &rhs)
  {
    std::clog << "assignment\n";
    if (this == &rhs) { return *this; }
    return *this;
  }

  Vocal &operator=(Vocal &&rhs) noexcept
  {
    std::clog << "move assignment\n";
    if (this == &rhs) { return *this; }
    return *this;
  }

  void hello() const
  {
    std::clog << "hello " << this << '\n';
  }
};
// NOLINTEND(hicpp-named-parameter,readability-named-parameter)

static void legacy(Vocal *ptr) { ptr->hello(); }

///////////////////////////////////////////////////////////////////////
// owner
///////////////////////////////////////////////////////////////////////

TEST_CASE("owner", "[owner]")
{
  gsl::owner<Vocal *> owner{ new Vocal() };
  owner->hello();
  legacy(owner);
  delete owner;
}

static void delete_me(gsl::owner<Vocal *> &owner)
{
  delete owner;
  owner = nullptr;
}

TEST_CASE("delete_me", "[owner]")
{
  gsl::owner<Vocal *> owner{ new Vocal() };
  delete_me(owner);
}

static void owner_copy(gsl::owner<Vocal *> thief) 
{
  std::clog << "thief=" << thief << '\n';
}

TEST_CASE("copy", "[owner]")
{
  gsl::owner<Vocal *> owner{ new Vocal() };

  // Logic error
	owner_copy(owner);

	delete_me(owner);
}

///////////////////////////////////////////////////////////////////////
// nonowner
///////////////////////////////////////////////////////////////////////

static void talk(mp::nonowner<Vocal *> nonowner) 
{ 
	nonowner->hello();
 }


static void owner_talk(gsl::owner<Vocal * const> const owner) 
{ 
	std::clog << "owner ";
	owner->hello();
 }

TEST_CASE("nonowner", "[nonowner]")
{
  gsl::owner<Vocal *> owner{ new Vocal() };
  mp::nonowner<Vocal *> nonowner = owner;
  talk(nonowner);
  legacy(nonowner);

	// Logic error but caught by clang tidy
	// NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
	owner_copy(nonowner);

	// Logic error but caught by clang tidy
	// NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
	owner_talk(nonowner);
#if 0
	// Logic error but caught by clang tidy
	// NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
	delete nonowner;
#else
delete owner;
#endif
}

///////////////////////////////////////////////////////////////////////
// std::unique_ptr
///////////////////////////////////////////////////////////////////////


TEST_CASE("uniqueptr", "[uniqueptr]")
{
  std::unique_ptr<Vocal>  owner{ new Vocal() };  // NOLINT ()
  mp::nonowner<Vocal *> nonowner = owner.get();
  talk(nonowner);
  legacy(nonowner);

	// Logic error but caught by clang tidy
	// NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
	owner_copy(nonowner);

	// Logic error but caught by clang tidy
	// NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
	owner_talk(nonowner);
#if 0
	// Logic error but caught by clang tidy
	// NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
	delete nonowner;
	owner.reset();
#endif
}


TEST_CASE("Constructor", "[constructor]") { SortedDoublyLinkedList const sut; }
