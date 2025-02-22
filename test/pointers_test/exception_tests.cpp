#include "marcpawl/pointers/exception.hpp"
#include <catch2/catch_test_macros.hpp>

namespace mp = marcpawl::pointers;

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)

TEST_CASE("default constructor", "[nullptr_exception]")
{
  mp::nullptr_exception const exception{};
  REQUIRE(std::string(exception.what()) == "");
}

TEST_CASE("explicit constructor", "[nullptr_exception]")
{
  mp::nullptr_exception const exception{ "message" };
  REQUIRE(std::string(exception.what()) == "message");
}

// NOLINTEND (cppcoreguidelines-avoid-magic-numbers)
