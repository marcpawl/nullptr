
#pragma once

#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_set>

#include <concepts>
#include <gsl/gsl>
#include <iostream>
#include <type_traits>
#include <utility>

#include "marcpawl/pointers/details.hpp"
#include "marcpawl/pointers/ptr.hpp"

namespace marcpawl {
namespace pointers {

  //
  // borrower
  //
  // Restricts the pointer to not be deleted.
  //
  // Has zero size overhead over T.
  //
  // If T is a pointer (i.e. T == U*) then
  // - allow construction from U*
  // - allow implicit conversion to U*
  //
  // based on gsl::not_null
  template<class T>
  using borrower = marcpawl::pointers::details::pointer<T,
    marcpawl::pointers::details::null_policy::nullable>;

  template<class T> borrower<T> make_borrower(T ptr) noexcept
  {
    return borrower<T>(ptr);
  }

  template<class T>
  using borrower_not_null = marcpawl::pointers::details::pointer<T,
    marcpawl::pointers::details::null_policy::not_null>;

  template<class T> borrower_not_null<T> make_borrower_not_null(T ptr) noexcept
  {
    return borrower_not_null<T>(ptr);
  }

}// namespace pointers
}// namespace marcpawl
