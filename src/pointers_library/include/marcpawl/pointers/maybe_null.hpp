
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

namespace marcpawl {
namespace pointers {
  template<class T, std::enable_if_t<std::is_pointer<T>::value, bool> = true>
  using maybe_null = T;

}// namespace pointers
}// namespace marcpawl
