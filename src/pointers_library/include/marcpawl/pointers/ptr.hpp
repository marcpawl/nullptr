
#pragma once

#include <concepts>
#include <cstddef>
#include <exception>
#include <functional>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <variant>

#include <gsl/gsl>

#include "marcpawl/pointers/details.hpp"

namespace marcpawl {
namespace pointers {
  template<typename T> class maybe_null;
  template<typename T> class strict_not_null;
}// namespace pointers
}// namespace marcpawl

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
}// namespace pointers
}// namespace marcpawl

namespace marcpawl {
namespace pointers {


  // Based on gsl::strict_not_null
  //
  // strict_not_null
  //
  // Restricts a pointer or smart pointer to only hold non-null values,
  //
  // - provides a strict (i.e. explicit constructor from T) wrapper of not_null
  // - to be used for new code that wishes the design to be cleaner and make
  // not_null
  //   checks intentional, or in old code that would like to make the
  //   transition.
  //
  //   To make the transition from not_null, incrementally replace not_null
  //   by strict_not_null and fix compilation errors
  //
  //   Expect to
  //   - remove all unneeded conversions from raw pointer to not_null and back
  //   - make API clear by specifying not_null in parameters where needed
  //   - remove unnecessary asserts
  //
  template<class T> class strict_not_null
  {
    static_assert(gsl::details::is_comparable_to_nullptr<T>::value,
      "T cannot be compared to nullptr.");

  private:
    T ptr_;

  public:
    strict_not_null() = delete;

  private:
    // Used to indicate that the is no construction from a pointer.
    struct privileged
    {
    };

    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    explicit constexpr strict_not_null(privileged const &, U &&u) noexcept(
      std::is_nothrow_move_constructible<T>::value)
      : ptr_(u)
    {}


  public:
    constexpr strict_not_null(strict_not_null<T> const &other)
      : ptr_(other.ptr_)
    {}

#if 0
    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    constexpr strict_not_null(const strict_not_null<U> &&other)
      : ptr_(other.ptr_)
    {}
#endif

    constexpr ~strict_not_null() = default;

    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    strict_not_null &operator=(strict_not_null<U> const &other)
    {
      ptr_ = other.ptr_;
      return *this;
    }

    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    strict_not_null &operator=(strict_not_null<U> &&other)
    {
      ptr_ = std::move(other.ptr_);
      return *this;
    }

  private:
    constexpr gsl::details::value_or_reference_return_t<T> get() const
      noexcept(noexcept(
        gsl::details::value_or_reference_return_t<T>{ std::declval<T &>() }))
    {
      return ptr_;
    }

  public:
    constexpr operator T() const { return get(); }
    constexpr decltype(auto) operator->() const { return get(); }
    constexpr decltype(auto) operator*() const { return *get(); }

    // unwanted operators...pointers only point to single objects!
    strict_not_null &operator++() = delete;
    strict_not_null &operator--() = delete;
    strict_not_null operator++(int) = delete;
    strict_not_null operator--(int) = delete;
    strict_not_null &operator+=(std::ptrdiff_t) = delete;
    strict_not_null &operator-=(std::ptrdiff_t) = delete;
    void operator[](std::ptrdiff_t) const = delete;

    template<typename U> friend class maybe_null;
  };

  // more unwanted operators
  template<class T, class U>
  std::ptrdiff_t operator-(strict_not_null<T> const &,
    strict_not_null<U> const &) = delete;
  template<class T>
  strict_not_null<T> operator-(strict_not_null<T> const &,
    std::ptrdiff_t) = delete;
  template<class T>
  strict_not_null<T> operator+(strict_not_null<T> const &,
    std::ptrdiff_t) = delete;
  template<class T>
  strict_not_null<T> operator+(std::ptrdiff_t,
    strict_not_null<T> const &) = delete;

#if (defined(__cpp_deduction_guides) && (__cpp_deduction_guides >= 201611L))

  // deduction guides to prevent the ctad-maybe-unsupported warning
  template<class T> strict_not_null(T) -> strict_not_null<T>;

#endif// ( defined(__cpp_deduction_guides) && (__cpp_deduction_guides >=
      // 201611L) )

}// namespace pointers
}// namespace marcpawl

namespace std {
template<class T>
struct hash<marcpawl::pointers::strict_not_null<T>>
  : gsl::not_null_hash<marcpawl::pointers::strict_not_null<T>>
{
};

}// namespace std

namespace marcpawl {
namespace pointers {


  //
  // maybe_null
  //
  // Holds a pointer, but has not pointer operations.  To get the pointer
  // use maybe_null#as_non_null()
  //
  // Has zero size overhead over T.
  //
  // If T is a pointer (i.e. T == U*) then
  // - allow construction from U*
  // - disallow construction from nullptr_t
  // - disallow default construction
  // - ensure construction from null U* fails
  // - allow implicit conversion to U*
  //
  template<class T> class maybe_null
  {
  public:
    static_assert(gsl::details::is_comparable_to_nullptr<T>::value,
      "T cannot be compared to nullptr.");

    using optional_not_null = std::optional<strict_not_null<T>>;
    using variant_not_null = std::variant<std::nullptr_t, strict_not_null<T>>;

    constexpr maybe_null() noexcept : ptr_(nullptr) {}

    constexpr maybe_null(std::nullptr_t) noexcept : ptr_(nullptr) {}


    /** Construct from a pointer. */
    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    explicit constexpr maybe_null(U &u) noexcept(
      std::is_nothrow_move_constructible<T>::value)
      : ptr_(std::forward<U>(u))
    {}

    /** Construct from a pointer. */
    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    constexpr maybe_null(U &&u) noexcept(
      std::is_nothrow_move_constructible<T>::value)
      : ptr_(std::forward<U>(u))
    {}

    /** Copy constructor */
    constexpr maybe_null(maybe_null<T> const &other) noexcept(
      std::is_nothrow_move_constructible<T>::value) = default;

    /** Copy constructor */
    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    constexpr maybe_null(maybe_null<U> const &other) noexcept(
      std::is_nothrow_move_constructible<T>::value)
      : maybe_null(other.ptr_)
    {}

    /** Move constructor */
    constexpr maybe_null(maybe_null<T> &&other) noexcept(
      std::is_nothrow_move_constructible<T>::value) = default;

    /** Move constructor */
    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    constexpr maybe_null(maybe_null<U> &&other) noexcept(
      std::is_nothrow_move_constructible<T>::value)
      : maybe_null(std::move(other.ptr_))
    {}

    ~maybe_null() = default;

    maybe_null &operator=(maybe_null const &other) = default;
    maybe_null &operator=(maybe_null &&other) = default;

    // unwanted operators...pointers only point to single objects!
    maybe_null &operator++() = delete;
    maybe_null &operator--() = delete;
    maybe_null operator++(int) = delete;
    maybe_null operator--(int) = delete;
    maybe_null &operator+=(std::ptrdiff_t) = delete;
    maybe_null &operator-=(std::ptrdiff_t) = delete;
    void operator[](std::ptrdiff_t) const = delete;

    constexpr bool operator!() const noexcept { return ptr_ == nullptr; }

    constexpr optional_not_null as_optional_not_null() const
    {
      if (ptr_ == nullptr) {
        return std::nullopt;
      } else {
        typename strict_not_null<T>::privileged privileged;
        strict_not_null<T> ptr{ privileged, ptr_ };
        return optional_not_null{ ptr };
      }
    }

    constexpr variant_not_null as_variant_not_null() const
    {
      if (ptr_ == nullptr) {
        return nullptr;
      } else {
        typename strict_not_null<T>::privileged privileged;
        strict_not_null<T> ptr{ privileged, ptr_ };
        return ptr;
      }
    }

    friend std::ostream &operator<<(std::ostream &os, maybe_null const &obj)
    {
      os << obj.ptr_;
      return os;
    }

    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    bool operator==(maybe_null<U> const &rhs) const
    {
      return (ptr_ == rhs.ptr_);
    }

    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    auto operator<=>(maybe_null<U> const &rhs) const
    {
      return (ptr_ <=> rhs.ptr_);
    }

    friend bool operator==(maybe_null const &lhs, void const *rhs)
    {
      return (lhs.ptr_ == rhs);
    }

    friend auto operator<=>(maybe_null const &lhs, void const *rhs)
    {
      void const *lhs_ptr = lhs.ptr_;
      return lhs_ptr <=> rhs;
    }

  private:
    constexpr gsl::details::value_or_reference_return_t<T> get() const
      noexcept(noexcept(
        gsl::details::value_or_reference_return_t<T>{ std::declval<T &>() }))
    {
      return ptr_;
    }

    T ptr_;

    template<typename U> friend class maybe_null;
  };


  template<class T> auto make_maybe_null(T &&t) noexcept
  {
    return maybe_null<std::remove_cv_t<std::remove_reference_t<T>>>{
      std::forward<T>(t)
    };
  }


  // more unwanted operators
  template<class T, class U>
  std::ptrdiff_t operator-(maybe_null<T> const &,
    maybe_null<U> const &) = delete;
  template<class T>
  maybe_null<T> operator-(maybe_null<T> const &, std::ptrdiff_t) = delete;
  template<class T>
  maybe_null<T> operator+(maybe_null<T> const &, std::ptrdiff_t) = delete;
  template<class T>
  maybe_null<T> operator+(std::ptrdiff_t, maybe_null<T> const &) = delete;


  template<class T,
    class U = decltype(std::declval<T const &>().get()),
    bool = std::is_default_constructible<std::hash<U>>::value>
  struct maybe_null_hash
  {
    std::size_t operator()(T const &value) const
    {
      return std::hash<U>{}(value.get());
    }
  };

  template<class T, class U> struct maybe_null_hash<T, U, false>
  {
    maybe_null_hash() = delete;
    maybe_null_hash(maybe_null_hash const &) = delete;
    maybe_null_hash &operator=(maybe_null_hash const &) = delete;
  };

}// namespace pointers
}// namespace marcpawl


namespace std {
template<class T>
struct hash<marcpawl::pointers::maybe_null<T>>
  : marcpawl::pointers::maybe_null_hash<marcpawl::pointers::maybe_null<T>>
{
};

}// namespace std


namespace marcpawl {
namespace pointers {
  namespace details {

    enum struct null_policy { nullable, not_null };
    enum struct ownership_policy { borrower, owner };

    template<typename T>
    concept Pointer = std::is_pointer_v<T>;

    template<Pointer T, null_policy nullable, ownership_policy ownership>
    class pointer
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
        null_policy rhs_nullable,
        typename = std::enable_if_t<std::is_convertible<U, T>::value>>
      explicit pointer(pointer<U, rhs_nullable, ownership> const &ptr) noexcept
        : ptr_(ptr.get())
      {
        // TODO not null will not accept nullable
        // TODO owner will not accept borrower
        // TODO borrower will accept owner
      }

      template<typename U,
        null_policy rhs_nullable,
        typename = std::enable_if_t<std::is_convertible<U, T>::value>>
      explicit pointer(pointer<U, rhs_nullable, ownership> &&other) noexcept
        : ptr_(other.get())
      {
        // TODO nullable accepts non-nullable
        // TODO borrower accepts owner
      }

      ~pointer() = default;

      template<typename U,
        typename = std::enable_if_t<std::is_convertible<U, T>::value>>
      pointer<T, nullable, ownership> &operator=(
        pointer<U, nullable, ownership> const &other) noexcept
      {
        // TODO non-nullable accepts nullable
        // TODO borrower accepts owner
        if (this == other) { return *this; }
        ptr_ = other.get();
        return *this;
      }

      constexpr T get() const noexcept { return ptr_; }
      constexpr decltype(auto) operator->() const
      {
        // TODO assert not nullptr
        return get();
      }
      constexpr decltype(auto) operator*() const
      {
        // TODO assert not nullptr
        return *get();
      }
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
    template<class T, null_policy nullable, ownership_policy ownership>
    std::ostream &operator<<(std::ostream &os,
      pointer<T, nullable, ownership> const &val)
    {
      os << val.get();
      return os;
    }
#endif// !defined(GSL_NO_IOSTREAMS)


    template<typename T,
      typename U,
      null_policy lhs_nullable,
      ownership_policy lhs_ownership,
      null_policy rhs_nullable,
      ownership_policy rhs_ownership>
      requires(::marcpawl::pointers::details::EqualityComparable<T, U>)
    auto operator==(pointer<T, lhs_nullable, lhs_ownership> const &lhs,
      pointer<U, rhs_nullable, rhs_ownership> const &rhs) noexcept
    {
      return lhs.get() == rhs.get();
    }

    // TODO
    // template <typename T, null_policy nullable>
    // auto operator==( pointer<T, nullable> const &lhs,  std::nullptr_t)
    // noexcept
    // {
    //   return lhs.get() == nullptr;
    // }

    template<typename T, null_policy nullable, ownership_policy ownership>
    auto operator==(pointer<T, nullable, ownership> const &lhs,
      void const *const rhs) noexcept
    {
      return lhs.get() == rhs;
    }

    template<typename T,
      typename U,
      null_policy lhs_nullable,
      ownership_policy lhs_ownership,
      null_policy rhs_nullable,
      ownership_policy rhs_ownership>
      requires(::marcpawl::pointers::details::EqualityComparable<T, U>)
    auto operator!=(pointer<T, lhs_nullable, lhs_ownership> const &lhs,
      pointer<U, rhs_nullable, rhs_ownership> const &rhs) noexcept
    {
      return lhs.get() != rhs.get();
    }

    template<typename T,
      typename U,
      null_policy lhs_nullable,
      ownership_policy lhs_ownership,
      null_policy rhs_nullable,
      ownership_policy rhs_ownership>
      requires(::marcpawl::pointers::details::Comparable<T, U>)
    auto operator<(pointer<T, lhs_nullable, lhs_ownership> const &lhs,
      pointer<U, rhs_nullable, rhs_ownership> const &rhs) noexcept
    {
      return lhs.get() < rhs.get();
    }

    template<typename T,
      typename U,
      null_policy lhs_nullable,
      ownership_policy lhs_ownership,
      null_policy rhs_nullable,
      ownership_policy rhs_ownership>
      requires(::marcpawl::pointers::details::Comparable<T, U>
               && ::marcpawl::pointers::details::EqualityComparable<T, U>)
    auto operator<=(pointer<T, lhs_nullable, lhs_ownership> const &lhs,
      pointer<U, rhs_nullable, rhs_ownership> const &rhs) noexcept
    {
      return lhs.get() <= rhs.get();
    }

    template<typename T,
      typename U,
      null_policy lhs_nullable,
      ownership_policy lhs_ownership,
      null_policy rhs_nullable,
      ownership_policy rhs_ownership>
      requires(::marcpawl::pointers::details::Comparable<T, U>)
    auto operator>(pointer<T, lhs_nullable, lhs_ownership> const &lhs,
      pointer<U, rhs_nullable, rhs_ownership> const &rhs) noexcept
    {
      auto l = lhs.get();
      auto r = rhs.get();
      return l > r;
    }

    template<typename T,
      typename U,
      null_policy lhs_nullable,
      ownership_policy lhs_ownership,
      null_policy rhs_nullable,
      ownership_policy rhs_ownership>
      requires(::marcpawl::pointers::details::Comparable<T, U>
               && ::marcpawl::pointers::details::EqualityComparable<T, U>)
    auto operator>=(pointer<T, lhs_nullable, lhs_ownership> const &lhs,
      pointer<U, rhs_nullable, rhs_ownership> const &rhs) noexcept
    {
      return lhs.get() >= rhs.get();
    }

    // more unwanted operators
    template<class T,
      class U,
      null_policy lhs_nullable,
      ownership_policy lhs_ownership,
      null_policy rhs_nullable,
      ownership_policy rhs_ownership>
    std::ptrdiff_t operator-(pointer<T, lhs_nullable, lhs_ownership> const &,
      pointer<U, rhs_nullable, rhs_ownership> const &) = delete;

    template<class T,
      null_policy lhs_nullable,
      ownership_policy lhs_ownership,
      null_policy rhs_nullable,
      ownership_policy rhs_ownership>
    pointer<T, lhs_nullable, lhs_ownership> operator-(
      pointer<T, rhs_nullable, rhs_ownership> const &,
      std::ptrdiff_t) = delete;

    template<class T,
      null_policy lhs_nullable,
      ownership_policy lhs_ownership,
      null_policy rhs_nullable,
      ownership_policy rhs_ownership>
    pointer<T, lhs_nullable, lhs_ownership> operator+(
      pointer<T, rhs_nullable, rhs_ownership> const &,
      std::ptrdiff_t) = delete;

    template<class T,
      null_policy lhs_nullable,
      ownership_policy lhs_ownership,
      null_policy rhs_nullable,
      ownership_policy rhs_ownership>
    pointer<T, lhs_nullable, lhs_ownership> operator+(std::ptrdiff_t,
      pointer<T, rhs_nullable, rhs_ownership> const &) = delete;

  }// namespace details


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
    marcpawl::pointers::details::null_policy::nullable,
    marcpawl::pointers::details::ownership_policy::borrower>;

  template<class T> borrower<T> make_borrower(T ptr) noexcept
  {
    return borrower<T>(ptr);
  }

  template<class T>
  using borrower_not_null = marcpawl::pointers::details::pointer<T,
    marcpawl::pointers::details::null_policy::not_null,
    marcpawl::pointers::details::ownership_policy::borrower>;

  template<class T> borrower_not_null<T> make_borrower_not_null(T ptr) noexcept
  {
    return borrower_not_null<T>(ptr);
  }

  template<class T>
  using owner = marcpawl::pointers::details::pointer<T,
    marcpawl::pointers::details::null_policy::nullable,
    marcpawl::pointers::details::ownership_policy::owner>;

  template<class T> owner<T> make_owner(T ptr) noexcept
  {
    return owner<T>(ptr);
  }

  template<class T>
  using owner_not_null = marcpawl::pointers::details::pointer<T,
    marcpawl::pointers::details::null_policy::not_null,
    marcpawl::pointers::details::ownership_policy::owner>;

  template<class T> owner_not_null<T> make_owner_not_null(T ptr) noexcept
  {
    return owner_not_null<T>(ptr);
  }

}// namespace pointers
}// namespace marcpawl


template<typename T,
  marcpawl::pointers::details::null_policy nullable,
  marcpawl::pointers::details::ownership_policy ownership>
struct std::hash<marcpawl::pointers::details::pointer<T, nullable, ownership>>
{
  std::size_t operator()(
    marcpawl::pointers::details::pointer<T, nullable, ownership> const &b)
    const noexcept
  {
    return std::hash<T>{}(b.get());
  }
};
