///////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//
// Goals
//
// Reduce the chance of following a null-pointer
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//
// Framework used for this talk.
//
// Chatty class that will be used to allocate on the heap.
//
///////////////////////////////////////////////////////////////////////

#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <optional>
#include <variant>

struct Vocal
{
  int data = 0;

  explicit Vocal(int data_) : data(data_)
  {
    std::clog << data << " constructor\n";
  }

  Vocal(Vocal const &other) : data(other.data)
  {
    std::clog << data << " copy constructor\n";
  }

  Vocal(Vocal &&other) noexcept : data(other.data)
  {
    std::clog << data << " move constructor\n";
  }

  ~Vocal() { std::clog << data << " destructor\n"; }

  Vocal &operator=(Vocal const &rhs)
  {
    std::clog << data << " assignment\n";
    if (this == &rhs) { return *this; }
    this->data = rhs.data;
    return *this;
  }

  Vocal &operator=(Vocal &&rhs) noexcept
  {
    std::clog << data << " move assignment\n";
    if (this == &rhs) { return *this; }
    this->data = rhs.data;
    return *this;
  }

  void hello() const { std::clog << data << " hello " << this << '\n'; }
};

static void legacy(Vocal *ptr) { ptr->hello(); }

#include "marcpawl/pointers/pointers.hpp"
namespace mp = marcpawl::pointers;

///////////////////////////////////////////////////////////////////////
// owner
///////////////////////////////////////////////////////////////////////

TEST_CASE("owner", "[owner]")
{
#if 0
  //  initializing non-owner 'Vocal *' with a newly created 'gsl::owner<>' [cppcoreguidelines-owning-memory,-warnings-as-errors]
  Vocal *vocalNullptr = nullptr;
#endif
  gsl::owner<Vocal *> owner{ new Vocal(__LINE__) };
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
  Vocal * unknown { new Vocal(__LINE__) };
  delete_me(unknown);
  gsl::owner<Vocal *> owner{ new Vocal(__LINE__) };
  delete_me(owner);
}

static void owner_copy(gsl::owner<Vocal *> thief)
{
  std::clog << "thief=" << thief << '\n';
  // we are allowed to delete thief since we own the pointer
}

TEST_CASE("copy", "[owner]")
{
  gsl::owner<Vocal *> owner{ new Vocal(__LINE__) };

  // Logic error
  owner_copy(owner);

  delete_me(owner);
}

///////////////////////////////////////////////////////////////////////
// mp::nonowner
///////////////////////////////////////////////////////////////////////

static void talk(mp::nonowner<Vocal *> nonowner) { nonowner->hello(); }


static void owner_talk(gsl::owner<Vocal *const> const owner)
{
  std::clog << "owner ";
  owner->hello();
}

TEST_CASE("nonowner", "[nonowner]")
{
  gsl::owner<Vocal *> owner{ new Vocal(__LINE__) };
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
//  mp::borrower
//
// Same as owner, but changes the errors from clang-tidy to compile 
// time
///////////////////////////////////////////////////////////////////////

TEST_CASE("borrower", "[borrower]")
{
  gsl::owner<Vocal *> owner{ new Vocal(__LINE__) };
  mp::borrower<Vocal *> const borrower{owner};

#if 0
  // no implcit conversion to raw pointer
  // explcitly deleted
  legacy(borrower);
#else
  legacy(borrower.get());
#endif

#if 0
  Vocal const * const raw = borrower;
  raw->hello();
#else
Vocal const * const raw = borrower.get();
raw->hello();
#endif

#if 0
	//  error: cannot delete expression of type 'mp::borrower<Vocal *>
	delete borrower;
#else
  delete owner;
#endif
}

 
///////////////////////////////////////////////////////////////////////
// std::unique_ptr
///////////////////////////////////////////////////////////////////////

TEST_CASE("uniqueptr", "[uniqueptr]")
{
  std::unique_ptr<Vocal> owner{ new Vocal(__LINE__) };// NOLINT ()
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

///////////////////////////////////////////////////////////////////////
// gsl::not_null
///////////////////////////////////////////////////////////////////////

void not_null_test(gsl::not_null<Vocal *> vocal) { vocal->hello(); }

void strict_null_ptr_test(gsl::strict_not_null<Vocal *> vocal)
{
  vocal->hello();
}

TEST_CASE("notnull", "[ptrs]")
{
  gsl::owner<Vocal *> vocal{ new Vocal(__LINE__) };
#if 0
  // will abort
  gsl::owner<Vocal *> vocalNullptr { nullptr };
  not_null_test(vocalNullptr);
#endif
  not_null_test(vocal);
  delete vocal;
}

TEST_CASE("notnull", "[owner_notnull]")
{
  gsl::not_null<gsl::owner<Vocal *>> const owner{ new Vocal(__LINE__) };
  gsl::not_null<mp::nonowner<Vocal *>> const nonowner_notnull{ owner };
  nonowner_notnull->hello();
  // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
  delete owner;
}

TEST_CASE("notnull", "[sharedptr_notnull]")
{
  gsl::not_null<std::unique_ptr<Vocal>> const unique =
    std::make_unique<Vocal>(__LINE__);
  unique->hello();
  gsl::not_null<std::shared_ptr<Vocal>> const owner =
    std::make_shared<Vocal>(__LINE__);
  owner->hello();
}

///////////////////////////////////////////////////////////////////////
// std::optional
///////////////////////////////////////////////////////////////////////

std::optional<gsl::not_null<std::shared_ptr<Vocal>>> optional_factory(int data)
{
  if (data % 2 == 0) {
    return std::nullopt;
  } else {
    return std::make_shared<Vocal>(data);
  }
}

TEST_CASE("notnull", "[optional_notnull]")
{
  std::optional<gsl::not_null<std::shared_ptr<Vocal>>> opt =
    optional_factory(__LINE__);
  // Stil a run time check but defined behaviour if we tried to use std::nullopt
  if (opt.has_value()) {
    opt.value()->hello();
  } else {
    std::clog << "no optional\n";
  }
}

///////////////////////////////////////////////////////////////////////
// std::variant
///////////////////////////////////////////////////////////////////////

std::variant<gsl::not_null<std::shared_ptr<Vocal>>, std::string>
  variant_factory(int data)
{
  if (data % 2 == 0) {
    return std::string{ "We only like odd things" };
  } else {
    return std::make_shared<Vocal>(data);
  }
}

template<class... Ts> struct overloaded : Ts...
{
  using Ts::operator()...;
};
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

TEST_CASE("notnull", "[variant_notnull]")
{
  auto variant = variant_factory(__LINE__);
  std::visit(
    overloaded{ [](std::string const &error) { std::clog << error << '\n'; },
      [](gsl::not_null<std::shared_ptr<Vocal>> const &vocal) {
        vocal->hello();
      } },
    variant);
}


TEST_CASE("notnull", "[variant_notnull_1]")
{
  auto variant = variant_factory(__LINE__);
  std::visit(
    [](auto &&arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, gsl::not_null<std::shared_ptr<Vocal>>>) {
        arg->hello();
      }
    },
    variant);
  // NO compiler diagnostic that std::string is not handled.
}
