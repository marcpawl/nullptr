
#pragma once

#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_set>

#include <concepts>
#include <utility>
#include <type_traits>
#include <iostream>
#include <gsl/gsl>

#include"marcpawl/pointers/details.hpp"
#include"marcpawl/pointers/ptr.hpp"

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
  template<class T> using borrower =  marcpawl::pointers::details::pointer<T, false>;

  template<class T>
  borrower<T> make_borrower(T ptr) noexcept
  {
    return borrower<T>(ptr);
  }

}// namespace pointers
}// namespace marcpawl
