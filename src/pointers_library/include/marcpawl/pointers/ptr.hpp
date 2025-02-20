
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

namespace marcpawl {
namespace pointers {
  namespace details {

    template<class T, bool nullable> class pointer
    {
    public:
      static_assert(std::is_pointer<T>::value, "T Must be pointer.");

      explicit pointer() requires (nullable)= default;

      explicit pointer(std::nullptr_t) noexcept requires (nullable)
	      : ptr_(nullptr)
      {}

      explicit pointer(T ptr) noexcept : ptr_(ptr) {}

      template<typename U,
        typename = std::enable_if_t<std::is_convertible<U, T>::value>>
      explicit pointer(pointer<U, nullable> const &ptr) noexcept
        : ptr_(ptr.get())
      {}

      template<typename U,
        typename = std::enable_if_t<std::is_convertible<U, T>::value>>
      explicit pointer(pointer<U, nullable> &&other) noexcept
        : ptr_(other.get())
      {}

      ~pointer() = default;

      template<typename U,
        typename = std::enable_if_t<std::is_convertible<U, T>::value>>
      pointer<T, nullable> &operator=(
        pointer<U, nullable> const &other) noexcept
      {
        if (this == other) { return *this; }
        ptr_ = other.get();
        return *this;
      }

      constexpr T get() const noexcept { return ptr_; }
      constexpr decltype(auto) operator->() const { return get(); }
      constexpr decltype(auto) operator*() const { return *get(); }
      constexpr operator bool() const { return get() != nullptr; }

      // unwanted operators...pointers only point to single objects!
      pointer &operator++() = delete;
      pointer &operator--() = delete;
      pointer operator++(int) = delete;
      pointer operator--(int) = delete;
      pointer &operator+=(std::ptrdiff_t) = delete;
      pointer &operator-=(std::ptrdiff_t) = delete;
      void operator[](std::ptrdiff_t) const = delete;

    private:
      T ptr_ = nullptr;
    };


#if !defined(GSL_NO_IOSTREAMS)
    template<class T, bool nullable>
    std::ostream &operator<<(std::ostream &os, pointer<T, nullable> const &val)
    {
      os << val.get();
      return os;
    }
#endif// !defined(GSL_NO_IOSTREAMS)


    template<typename T, typename U, bool nullable>
      requires(::marcpawl::pointers::details::EqualityComparable<T, U>)
    auto operator==(pointer<T, nullable> const &lhs,
      pointer<U, nullable> const &rhs) noexcept
    {
      return lhs.get() == rhs.get();
    }

    // template <typename T, bool nullable>
    // auto operator==( pointer<T, nullable> const &lhs,  std::nullptr_t)
    // noexcept
    // {
    //   return lhs.get() == nullptr;
    // }

    template<typename T, bool nullable>
    auto operator==(pointer<T, nullable> const &lhs,
      void const *const rhs) noexcept
    {
      return lhs.get() == rhs;
    }

    template<typename T, typename U, bool nullable>
      requires(::marcpawl::pointers::details::EqualityComparable<T, U>)
    auto operator!=(pointer<T, nullable> const &lhs,
      pointer<U, nullable> const &rhs) noexcept
    {
      return lhs.get() != rhs.get();
    }

    template<typename T, typename U, bool nullable>
      requires(::marcpawl::pointers::details::Comparable<T, U>)
    auto operator<(pointer<T, nullable> const &lhs,
      pointer<U, nullable> const &rhs) noexcept
    {
      return lhs.get() < rhs.get();
    }

    template<typename T, typename U, bool nullable>
      requires(::marcpawl::pointers::details::Comparable<T, U>
               && ::marcpawl::pointers::details::EqualityComparable<T, U>)
    auto operator<=(pointer<T, nullable> const &lhs,
      pointer<U, nullable> const &rhs) noexcept
    {
      return lhs.get() <= rhs.get();
    }

    template<typename T, typename U, bool nullable>
      requires(::marcpawl::pointers::details::Comparable<T, U>)
    auto operator>(pointer<T, nullable> const &lhs,
      pointer<U, nullable> const &rhs) noexcept
    {
      return lhs.get() > rhs.get();
    }

    template<typename T, typename U, bool nullable>
      requires(::marcpawl::pointers::details::Comparable<T, U>
               && ::marcpawl::pointers::details::EqualityComparable<T, U>)
    auto operator>=(pointer<T, nullable> const &lhs,
      pointer<U, nullable> const &rhs) noexcept
    {
      return lhs.get() >= rhs.get();
    }

    // more unwanted operators
    template<class T, class U, bool nullable>
    std::ptrdiff_t operator-(const pointer<T, nullable> &,
      const pointer<U, nullable> &) = delete;
    template<class T, bool nullable>
    pointer<T, nullable> operator-(const pointer<T, nullable> &,
      std::ptrdiff_t) = delete;
    template<class T, bool nullable>
    pointer<T, nullable> operator+(const pointer<T, nullable> &,
      std::ptrdiff_t) = delete;
    template<class T, bool nullable>
    pointer<T, nullable> operator+(std::ptrdiff_t,
      const pointer<T, nullable> &) = delete;

  }// namespace details
}// namespace pointers
}// namespace marcpawl


template<typename T, bool nullable>
struct std::hash<marcpawl::pointers::details::pointer<T, nullable>>
{
  std::size_t operator()(
    const marcpawl::pointers::details::pointer<T, nullable> &b) const noexcept
  {
    return std::hash<T>{}(b.get());
  }
};

