
#pragma once

#include <cstddef>
#include <exception>
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

  class nullptr_exception : public std::exception
  {
  public:
    explicit nullptr_exception() {}
    explicit nullptr_exception(std::string const &message) : message_(message)
    {}
    ~nullptr_exception() noexcept override = default;

    [[nodiscard]] char const *what() const noexcept override
    {
      return message_.c_str();
    }

  private:
    std::string message_;
  };

  namespace details {

    enum struct null_policy { nullable, not_null };

    template<class T, null_policy nullable> class pointer
    {
    public:
      static_assert(std::is_pointer<T>::value, "T Must be pointer.");

      explicit pointer()
        requires(null_policy::nullable == nullable)
      = default;

      explicit pointer(std::nullptr_t) noexcept : ptr_(nullptr)
      {
        static_assert(
          nullable == null_policy::nullable, "parameter cannot be nullptr");
      }

      explicit pointer(T ptr) noexcept
        requires(nullable == null_policy::nullable)
        : ptr_(ptr)
      {}

      explicit pointer(T ptr)
        requires(nullable == null_policy::not_null)
        : ptr_(ptr)
      {
        if (ptr == nullptr) { throw nullptr_exception(); }
      }

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
    template<class T, null_policy nullable>
    std::ostream &operator<<(std::ostream &os, pointer<T, nullable> const &val)
    {
      os << val.get();
      return os;
    }
#endif// !defined(GSL_NO_IOSTREAMS)


    template<typename T,
      typename U,
      null_policy lhs_nullable,
      null_policy rhs_nullable>
      requires(::marcpawl::pointers::details::EqualityComparable<T, U>)
    auto operator==(pointer<T, lhs_nullable> const &lhs,
      pointer<U, rhs_nullable> const &rhs) noexcept
    {
      return lhs.get() == rhs.get();
    }

    // template <typename T, null_policy nullable>
    // auto operator==( pointer<T, nullable> const &lhs,  std::nullptr_t)
    // noexcept
    // {
    //   return lhs.get() == nullptr;
    // }

    template<typename T, null_policy nullable>
    auto operator==(pointer<T, nullable> const &lhs,
      void const *const rhs) noexcept
    {
      return lhs.get() == rhs;
    }

    template<typename T,
      typename U,
      null_policy lhs_nullable,
      null_policy rhs_nullable>
      requires(::marcpawl::pointers::details::EqualityComparable<T, U>)
    auto operator!=(pointer<T, lhs_nullable> const &lhs,
      pointer<U, rhs_nullable> const &rhs) noexcept
    {
      return lhs.get() != rhs.get();
    }

    template<typename T,
      typename U,
      null_policy lhs_nullable,
      null_policy rhs_nullable>
      requires(::marcpawl::pointers::details::Comparable<T, U>)
    auto operator<(pointer<T, lhs_nullable> const &lhs,
      pointer<U, rhs_nullable> const &rhs) noexcept
    {
      return lhs.get() < rhs.get();
    }

    template<typename T,
      typename U,
      null_policy lhs_nullable,
      null_policy rhs_nullable>
      requires(::marcpawl::pointers::details::Comparable<T, U>
               && ::marcpawl::pointers::details::EqualityComparable<T, U>)
    auto operator<=(pointer<T, lhs_nullable> const &lhs,
      pointer<U, rhs_nullable> const &rhs) noexcept
    {
      return lhs.get() <= rhs.get();
    }

    template<typename T,
      typename U,
      null_policy lhs_nullable,
      null_policy rhs_nullable>
      requires(::marcpawl::pointers::details::Comparable<T, U>)
    auto operator>(pointer<T, lhs_nullable> const &lhs,
      pointer<U, rhs_nullable> const &rhs) noexcept
    {
      auto l = lhs.get();
      auto r = rhs.get();
      return l > r;
    }

    template<typename T,
      typename U,
      null_policy lhs_nullable,
      null_policy rhs_nullable>
      requires(::marcpawl::pointers::details::Comparable<T, U>
               && ::marcpawl::pointers::details::EqualityComparable<T, U>)
    auto operator>=(pointer<T, lhs_nullable> const &lhs,
      pointer<U, rhs_nullable> const &rhs) noexcept
    {
      return lhs.get() >= rhs.get();
    }

    // more unwanted operators
    template<class T,
      class U,
      null_policy lhs_nullable,
      null_policy rhs_nullable>
    std::ptrdiff_t operator-(const pointer<T, lhs_nullable> &,
      const pointer<U, rhs_nullable> &) = delete;

    template<class T, null_policy lhs_nullable, null_policy rhs_nullable>
    pointer<T, lhs_nullable> operator-(const pointer<T, rhs_nullable> &,
      std::ptrdiff_t) = delete;

    template<class T, null_policy lhs_nullable, null_policy rhs_nullable>
    pointer<T, lhs_nullable> operator+(const pointer<T, rhs_nullable> &,
      std::ptrdiff_t) = delete;

    template<class T, null_policy lhs_nullable, null_policy rhs_nullable>
    pointer<T, lhs_nullable> operator+(std::ptrdiff_t,
      const pointer<T, rhs_nullable> &) = delete;

  }// namespace details
}// namespace pointers
}// namespace marcpawl


template<typename T, marcpawl::pointers::details::null_policy nullable>
struct std::hash<marcpawl::pointers::details::pointer<T, nullable>>
{
  std::size_t operator()(
    const marcpawl::pointers::details::pointer<T, nullable> &b) const noexcept
  {
    return std::hash<T>{}(b.get());
  }
};
