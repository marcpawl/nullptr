
#pragma once

#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_set>

#include "details.hpp"
#include <concepts>
#include <gsl/gsl>
#include <iostream>
#include <type_traits>
#include <utility>

namespace marcpawl {
namespace pointers {

#if 0
  //
  // owner
  //
  // Has zero size overhead over T.
  //
  // If T is a pointer (i.e. T == U*) then
  // - allow construction from U*
  // - allow implicit conversion to U*
  //
  // based on gsl::not_null
  template<class T> class owner
  {
  public:
    static_assert(std::is_pointer<T>::value, "T Must be pointer.");

    explicit owner() = default;

    explicit owner(T ptr) noexcept : ptr_(ptr) {}

    template<typename U>
    explicit owner(owner<U> const &ptr) = delete;

    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    explicit owner(owner<U> &&other) noexcept : ptr_(other.get())
    {
	    other.ptr_ = nullptr;
    }

    ~owner() = default;

    template<typename U>
    owner<T> &operator=(owner<U> const &other) = delete;

    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    owner<T>& owner(owner<U> &&other) noexcept : ptr_(other.get())
    {
	    other.ptr_ = nullptr;
    }
    constexpr T get() const noexcept { return ptr_; }
    constexpr decltype(auto) operator->() const { return get(); }
    constexpr decltype(auto) operator*() const { return *get(); }

    operator void*() const { return ptr_; }

    // unwanted operators...pointers only point to single objects!
    owner &operator++() = delete;
    owner &operator--() = delete;
    owner operator++(int) = delete;
    owner operator--(int) = delete;
    owner &operator+=(std::ptrdiff_t) = delete;
    owner &operator-=(std::ptrdiff_t) = delete;
    void operator[](std::ptrdiff_t) const = delete;

    void operator delete(void *, size_t) = delete;

  private:
    T ptr_ = nullptr;
  };

  template<typename T, typename U,
  typename = std::enable_if_t<std::is_convertible<U, T>::value>> auto make_owner(U &&u) noexcept
  {
    return owner<std::remove_cv_t<std::remove_reference_t<T>>>{
      std::forward<T>(u)
    };
  }

#if !defined(GSL_NO_IOSTREAMS)
  template<class T>
  std::ostream &operator<<(std::ostream &os,  owner<T> const &val)
  {
    os << val.get();
    return os;
  }
#endif// !defined(GSL_NO_IOSTREAMS)

template <typename T, typename U>
requires (details::EqualityComparable<T, U>)
  auto operator==( owner<T> const &lhs,  owner<U> const &rhs) noexcept
  {
    return lhs.get() == rhs.get();
  }

  template <typename T>
  auto operator==( owner<T> const &lhs,  std::nullptr_t) noexcept
  {
    return lhs.get() == nullptr;
  }

  template <typename T, typename U>
requires (details::EqualityComparable<T, U>)
  auto operator!=( owner<T> const &lhs,  owner<U> const &rhs) noexcept
  {
    return lhs.get() != rhs.get();
  }

  template <typename T, typename U>
  requires (details::Comparable<T, U>)
    auto operator<( owner<T> const &lhs,  owner<U> const &rhs) noexcept
    {
      return lhs.get() < rhs.get();
    }

    template <typename T, typename U>
    requires (details::Comparable<T, U> && details::EqualityComparable<T, U>)
      auto operator<=( owner<T> const &lhs,  owner<U> const &rhs) noexcept
      {
        return lhs.get() <= rhs.get();
      }
  
    template <typename T, typename U>
    requires (details::Comparable<T, U>)
      auto operator>( owner<T> const &lhs,  owner<U> const &rhs) noexcept
      {
        return lhs.get() > rhs.get();
      }

      template <typename T, typename U>
      requires (details::Comparable<T, U> && details::EqualityComparable<T, U>)
      auto operator>=( owner<T> const &lhs,  owner<U> const &rhs) noexcept
        {
          return lhs.get() >= rhs.get();
        }

  // more unwanted operators
  template<class T, class U>
  std::ptrdiff_t operator-(const owner<T> &, const owner<U> &) = delete;
  template<class T>
  owner<T> operator-(const owner<T> &, std::ptrdiff_t) = delete;
  template<class T>
  owner<T> operator+(const owner<T> &, std::ptrdiff_t) = delete;
  template<class T>
  owner<T> operator+(std::ptrdiff_t, const owner<T> &) = delete;

#endif
}// namespace pointers
}// namespace marcpawl

#if 0

template<typename T>
struct std::hash<marcpawl::pointers::owner<T>>
{
    std::size_t operator()(const marcpawl::pointers::owner<T>& b) const noexcept
    {
        return std::hash<T>{}(b.get());
    }
};
#endif
