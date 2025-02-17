
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
  template<class T> class borrower
  {
  public:
    static_assert(std::is_pointer<T>::value, "T Must be pointer.");

    explicit borrower() = default;

    explicit borrower(T ptr) noexcept : ptr_(ptr) {}

    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    explicit borrower(borrower<U> const &ptr) noexcept : ptr_(ptr.get())
    {}

    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    explicit borrower(borrower<U> &&other) noexcept : ptr_(other.get())
    {}

    ~borrower() = default;

    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    borrower<T> &operator=(borrower<U> const &other) noexcept
    {
      if (this == other) { return *this; }
      ptr_ = other.get();
      return *this;
    }

    constexpr T get() const noexcept { return ptr_; }
    constexpr decltype(auto) operator->() const { return get(); }
    constexpr decltype(auto) operator*() const { return *get(); }

    operator void*() const { return ptr_; }

    // unwanted operators...pointers only point to single objects!
    borrower &operator++() = delete;
    borrower &operator--() = delete;
    borrower operator++(int) = delete;
    borrower operator--(int) = delete;
    borrower &operator+=(std::ptrdiff_t) = delete;
    borrower &operator-=(std::ptrdiff_t) = delete;
    void operator[](std::ptrdiff_t) const = delete;

    void operator delete(void *, size_t) = delete;

  private:
    T ptr_ = nullptr;
  };

  template<typename T, typename U,
  typename = std::enable_if_t<std::is_convertible<U, T>::value>> auto make_borrower(U &&u) noexcept
  {
    return borrower<std::remove_cv_t<std::remove_reference_t<T>>>{
      std::forward<T>(u)
    };
  }

#if !defined(GSL_NO_IOSTREAMS)
  template<class T>
  std::ostream &operator<<(std::ostream &os,  borrower<T> const &val)
  {
    os << val.get();
    return os;
  }
#endif// !defined(GSL_NO_IOSTREAMS)


template <typename T, typename U>
requires (::marcpawl::pointers::details::EqualityComparable<T, U>)
  auto operator==( borrower<T> const &lhs,  borrower<U> const &rhs) noexcept
  {
    return lhs.get() == rhs.get();
  }

  template <typename T>
  auto operator==( borrower<T> const &lhs,  std::nullptr_t) noexcept
  {
    return lhs.get() == nullptr;
  }

  template <typename T, typename U>
requires (::marcpawl::pointers::details::EqualityComparable<T, U>)
  auto operator!=( borrower<T> const &lhs,  borrower<U> const &rhs) noexcept
  {
    return lhs.get() != rhs.get();
  }

  template <typename T, typename U>
  requires (::marcpawl::pointers::details::Comparable<T, U>)
    auto operator<( borrower<T> const &lhs,  borrower<U> const &rhs) noexcept
    {
      return lhs.get() < rhs.get();
    }

    template <typename T, typename U>
    requires (::marcpawl::pointers::details::Comparable<T, U> && ::marcpawl::pointers::details::EqualityComparable<T, U>)
      auto operator<=( borrower<T> const &lhs,  borrower<U> const &rhs) noexcept
      {
        return lhs.get() <= rhs.get();
      }
  
    template <typename T, typename U>
    requires (::marcpawl::pointers::details::Comparable<T, U>)
      auto operator>( borrower<T> const &lhs,  borrower<U> const &rhs) noexcept
      {
        return lhs.get() > rhs.get();
      }

      template <typename T, typename U>
      requires (::marcpawl::pointers::details::Comparable<T, U> && ::marcpawl::pointers::details::EqualityComparable<T, U>)
      auto operator>=( borrower<T> const &lhs,  borrower<U> const &rhs) noexcept
        {
          return lhs.get() >= rhs.get();
        }

  // more unwanted operators
  template<class T, class U>
  std::ptrdiff_t operator-(const borrower<T> &, const borrower<U> &) = delete;
  template<class T>
  borrower<T> operator-(const borrower<T> &, std::ptrdiff_t) = delete;
  template<class T>
  borrower<T> operator+(const borrower<T> &, std::ptrdiff_t) = delete;
  template<class T>
  borrower<T> operator+(std::ptrdiff_t, const borrower<T> &) = delete;

}// namespace pointers
}// namespace marcpawl


template<typename T>
struct std::hash<marcpawl::pointers::borrower<T>>
{
    std::size_t operator()(const marcpawl::pointers::borrower<T>& b) const noexcept
    {
        return std::hash<T>{}(b.get());
    }
};
