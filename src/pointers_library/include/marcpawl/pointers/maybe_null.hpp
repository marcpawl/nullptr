
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

namespace marcpawl {
namespace pointers {
  template<class T, std::enable_if_t<std::is_pointer<T>::value, bool> = true>
  using maybe_null = T;

}// namespace pointers
}// namespace marcpawl
