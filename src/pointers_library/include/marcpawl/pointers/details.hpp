
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
  namespace details {

    template<typename T, typename U>
    concept EqualityComparable = requires(T a, U b) {
      { a == b } -> std::convertible_to<bool>;
    };

    template<typename T, typename U>
    concept Comparable = requires(T a, U b) {
      { a < b } -> std::convertible_to<bool>;
    };


  }// namespace details
}// namespace pointers
}// namespace marcpawl
