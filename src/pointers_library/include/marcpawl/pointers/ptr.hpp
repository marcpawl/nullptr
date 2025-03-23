
#pragma once

#include <concepts>
#include <cstddef>
#include <exception>
#include <functional>
#include <iomanip>
#if !defined(MP_NO_IOSTREAMS)
#include <iostream>
#endif
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <variant>

namespace marcpawl {
namespace pointers {
  namespace details {
    // Resolves to the more efficient of `const T` or `const T&`, in the context
    // of returning a const-qualified value of type T.
    //
    // Copied from cppfront's implementation of the CppCoreGuidelines F.16
    // (https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-in)
    //
    // Copied from
    // https://github.com/microsoft/GSL/blob/main/include/gsl/pointers
    // User MIT license
    template<typename T>
    using value_or_reference_return_t =
      std::conditional_t<sizeof(T) < 2 * sizeof(void *)
                           && std::is_trivially_copy_constructible<T>::value,
        T const,
        T const &>;

    template<typename T, typename U>
    concept EqualityComparable = requires(T a, U b) {
      { a == b } -> std::convertible_to<bool>;
    };

    template<typename T, typename U>
    concept Comparable = requires(T a, U b) {
      { a < b } -> std::convertible_to<bool>;
    };


  }// namespace details

  template<typename T>
  concept not_nullptr =
    !std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>,
      std::nullptr_t>;

  template<typename T>
  concept Pointer = requires(T p) {
    *p;// T must be dereferenceable
    {
      p == nullptr
    } -> std::convertible_to<bool>;// T must be comparable to nullptr
  };

  template<typename T>
  concept Nullable = requires(T t) { t = nullptr; };

  template<typename T>
  concept VoidComparable = requires(T t, void *p) { t == p; };

  template<Pointer T> class maybe_null;
  template<Pointer T> class strict_not_null;
  template<Pointer T> class owner;
  template<Pointer T> class borrower;

  template<typename F>
  concept nullptr_handler = std::invocable<F, std::nullptr_t>;

  template<typename F, typename T>
  concept not_null_handler = std::invocable<F, strict_not_null<T>>;

  template<Pointer T> class wrapped_pointer;


  // Define a concept that checks if U is not a wrapped_pointer or derived from
  // it
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

  ////////////////////////////////////////////////////////////////////////////
  // wrapped_pointer
  //
  // Pointer that will reject use as a non-single type.
  // If the type is not single, use std::span.
  ////////////////////////////////////////////////////////////////////////////

  struct wrapped_pointer_base
  {
    ~wrapped_pointer_base() = default;
  };

  template<Pointer T> class wrapped_pointer : public wrapped_pointer_base
  {
  public:
    T ptr_;

  public:
    wrapped_pointer() : ptr_(nullptr) {}
    wrapped_pointer(T const &ptr)
      requires std::is_copy_constructible_v<T>
      : ptr_(ptr)
    {}

    wrapped_pointer(T &&ptr)
      requires std::is_move_constructible_v<T>
      : ptr_(std::move(ptr))
    {}

    // Use get().  Deleted to avoid conversion to raw pointer,
    // and then construction into a different wrapped pointer
    // type, avoiding the type check.
    // operator T() const  = delete;

    constexpr details::value_or_reference_return_t<T> get() const
    {
      return ptr_;
    }


    // Define three-way comparison for wrapped_pointer and wrapped_pointer
    auto operator<=>(wrapped_pointer const &other) const
    {
      return ptr_ <=> other.ptr_;
    }


    // // Define three-way comparison for wrapped_pointer<T> and
    // wrapped_pointer<U> template <typename U> auto operator<=>(const
    // wrapped_pointer<U>& rhs) const {
    //     // Implement comparison logic for wrapped_pointer<T> and
    //     wrapped_pointer<U> return ptr_ <=> rhs;
    // }

    // unwanted operators...pointers only point to single objects!
    wrapped_pointer &operator++() = delete;
    wrapped_pointer &operator--() = delete;
    wrapped_pointer operator++(int) = delete;
    wrapped_pointer operator--(int) = delete;
    wrapped_pointer &operator+=(std::ptrdiff_t) = delete;
    wrapped_pointer &operator-=(std::ptrdiff_t) = delete;
    void operator[](std::ptrdiff_t) const = delete;
  };


  template<typename T> auto operator<(wrapped_pointer<T> const &lhs, void *rhs)
  {
    return lhs.get() < rhs;
  }

  template<typename T> auto operator<=(wrapped_pointer<T> const &lhs, void *rhs)
  {
    return lhs.get() <= rhs;
  }

  template<typename T> auto operator>=(wrapped_pointer<T> const &lhs, void *rhs)
  {
    return lhs.get() >= rhs;
  }

  template<typename T, typename U>
  auto operator>(wrapped_pointer<T> const &lhs, wrapped_pointer<U> const &rhs)
  {
    return lhs.get() > rhs.get();
  }

  template<typename T> auto operator>(wrapped_pointer<T> const &lhs, void *rhs)
  {
    return lhs.get() > rhs;
  }

  template<typename T, typename U>
  auto operator<(wrapped_pointer<T> const &lhs, wrapped_pointer<U> const &rhs)
  {
    return lhs.get() < rhs.get();
  }

  template<typename T>
  auto operator<(void const *const lhs, wrapped_pointer<T> const &rhs)
  {
    return lhs < rhs.get();
  }

  template<typename T, typename U>
  auto operator<=(wrapped_pointer<T> const &lhs, wrapped_pointer<U> const &rhs)
  {
    return lhs.get() <= rhs.get();
  }

  template<typename T> auto operator<=(void *lhs, wrapped_pointer<T> const &rhs)
  {
    return lhs <= rhs.get();
  }

  template<typename T, typename U>
  auto operator>=(wrapped_pointer<T> const &lhs, wrapped_pointer<U> const &rhs)
  {
    return lhs.get() >= rhs.get();
  }

  template<typename T> auto operator>=(void *lhs, wrapped_pointer<T> const &rhs)
  {
    return lhs >= rhs.get();
  }

  template<typename T> auto operator>(void *lhs, wrapped_pointer<T> const &rhs)
  {
    return lhs > rhs.get();
  }

  template<typename T>
    requires VoidComparable<T>
  [[nodiscard]] auto operator==(wrapped_pointer<T> const &lhs,
    void const *const rhs);

  template<typename T>
    requires VoidComparable<T>
  [[nodiscard]] auto operator==(void const *const lhs,
    wrapped_pointer<T> const &rhs);

  template<typename T, typename U>
  [[nodiscard]] auto operator==(wrapped_pointer<T> const &lhs,
    wrapped_pointer<U> const &rhs)
  {
    auto const &lhs_value = lhs.get();
    auto const &rhs_value = rhs.get();
    return lhs_value == rhs_value;
  }

  template<typename T>
    requires VoidComparable<T>
  [[nodiscard]] auto operator==(wrapped_pointer<T> const &lhs,
    void const *const rhs)
  {
    return lhs.get() == rhs;
  }


  template<typename T>
    requires VoidComparable<T>
  [[nodiscard]] auto operator==(void const *const lhs,
    wrapped_pointer<T> const &rhs)
  {
    return lhs == rhs.get();
    ;
  }

  template<typename T>
  [[nodiscard]] bool operator!(wrapped_pointer<T> const &ptr) noexcept
  {
    return ptr.get() == nullptr;
  }

  // more unwanted operators
  template<class T, class U>
  std::ptrdiff_t operator-(wrapped_pointer<T> const &,
    wrapped_pointer<U> const &) = delete;
  template<class T>
  wrapped_pointer<T> operator-(wrapped_pointer<T> const &,
    std::ptrdiff_t) = delete;
  template<class T>
  wrapped_pointer<T> operator+(wrapped_pointer<T> const &,
    std::ptrdiff_t) = delete;
  template<class T>
  wrapped_pointer<T> operator+(std::ptrdiff_t,
    wrapped_pointer<T> const &) = delete;

#if !defined(MP_NO_IOSTREAMS)
  template<class T>
  std::ostream &operator<<(std::ostream &os, wrapped_pointer<T> const &val)
  {
    os << val.get();
    return os;
  }
#endif// !defined(MP_NO_IOSTREAMS)


#ifdef TODO
  template<typename T, typename U>
    requires(::marcpawl::pointers::details::Comparable<T, U>
             && ::marcpawl::pointers::details::EqualityComparable<T, U>)
  auto operator<=(wrapped_pointer<T> const &lhs,
    wrapped_pointer<U> const &rhs) noexcept
  {
    return lhs.get() <= rhs.get();
  }

  template<typename T, typename U>
    requires(::marcpawl::pointers::details::Comparable<T, U>)
  auto operator>(wrapped_pointer<T> const &lhs,
    wrapped_pointer<U> const &rhs) noexcept
  {
    auto l = lhs.get();
    auto r = rhs.get();
    return l > r;
  }

  template<typename T, typename U>
    requires(::marcpawl::pointers::details::Comparable<T, U>
             && ::marcpawl::pointers::details::EqualityComparable<T, U>)
  auto operator>=(wrapped_pointer<T> const &lhs,
    wrapped_pointer<U> const &rhs) noexcept
  {
    return lhs.get() >= rhs.get();
  }
#endif


  ////////////////////////////////////////////////////////////////////////////
  // strict_not_null
  //
  // Based on gsl::strict_not_null
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
  ////////////////////////////////////////////////////////////////////////////
  template<Pointer T> class strict_not_null : public wrapped_pointer<T>
  {

  public:
    strict_not_null() = delete;

    template<Pointer U>
    constexpr explicit strict_not_null(U &&u) noexcept(
      std::is_nothrow_move_constructible<T>::value)
      : wrapped_pointer<T>(std::move(u))
    {
      if (this->ptr_ == nullptr) { throw nullptr_exception(); }
    }


  public:
    constexpr strict_not_null(strict_not_null<T> const &other)
      requires std::is_copy_constructible_v<T>
      : wrapped_pointer<T>(other.ptr_)
    {}

    constexpr strict_not_null(strict_not_null<T> &&other)
      requires std::is_move_constructible_v<T>
      : wrapped_pointer<T>(std::move(other.ptr_))
    {}

    constexpr ~strict_not_null() = default;

    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    constexpr strict_not_null &operator=(strict_not_null<U> const &other)
    {
      this->ptr_ = other.ptr_;
      return *this;
    }

    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    constexpr strict_not_null &operator=(strict_not_null<U> &&other)
    {
      this->ptr_ = std::move(other.ptr_);
      return *this;
    }


  public:
    constexpr decltype(auto) operator->() const { return this->get(); }
    constexpr decltype(auto) operator*() const { return *(this->get()); }


    template<Pointer U> friend class maybe_null;
  };


#if (defined(__cpp_deduction_guides) && (__cpp_deduction_guides >= 201611L))

  // deduction guides to prevent the ctad-maybe-unsupported warning
  template<class T> strict_not_null(T) -> strict_not_null<T>;

#endif// ( defined(__cpp_deduction_guides) && (__cpp_deduction_guides >=
      // 201611L) )


  ////////////////////////////////////////////////////////////////////////////
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
  ////////////////////////////////////////////////////////////////////////////
  template<Pointer T> class maybe_null : public wrapped_pointer<T>
  {
  public:
    using optional_not_null = std::optional<strict_not_null<T>>;
    using variant_not_null = std::variant<std::nullptr_t, strict_not_null<T>>;

    constexpr maybe_null() noexcept : wrapped_pointer<T>(nullptr) {}

    explicit maybe_null(std::nullptr_t) noexcept : wrapped_pointer<T>(nullptr)
    {}


    /** Construct from a pointer. */
    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    explicit constexpr maybe_null(U const &u)
      requires std::is_copy_constructible_v<T>
      : wrapped_pointer<T>(u)
    {}

    /** Construct from a pointer. */
    explicit constexpr maybe_null(T &&t)
      requires std::is_move_constructible_v<T>
      : wrapped_pointer<T>(std::move(t))
    {}

    /** Copy constructor */
    maybe_null(maybe_null<T> const &other) noexcept(
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

    [[nodiscard]] constexpr optional_not_null as_optional_not_null() const &
    {
      if (this->ptr_ == nullptr) {
        return std::nullopt;
      } else {
        strict_not_null<T> ptr{ this->ptr_ };
        return optional_not_null{ ptr };
      }
    }

    [[nodiscard]] constexpr optional_not_null as_optional_not_null() &&
    {
      if (this->ptr_ == nullptr) {
        return std::nullopt;
      } else {
        strict_not_null<T> ptr{ std::move(this->ptr_) };
        return optional_not_null{ std::move(ptr) };
      }
    }

    [[nodiscard]] constexpr variant_not_null as_variant_not_null() const
    {
      if (this->ptr_ == nullptr) {
        return nullptr;
      } else {
        strict_not_null<T> ptr{ this->ptr_ };
        return ptr;
      }
    }

    [[nodiscard]] constexpr auto visit(nullptr_handler auto handle_nullptr,
      not_null_handler<T> auto handle_not_null) const
    {
      if (this->ptr_ == nullptr) {
        return handle_nullptr(nullptr);
      } else {
        strict_not_null<T> ptr{ this->ptr_ };
        return handle_not_null(std::move(ptr));
      }
    }

    friend constexpr std::ostream &operator<<(std::ostream &os,
      maybe_null const &obj)
    {
      os << obj.ptr_;
      return os;
    }


    [[deprecated]] [[nodiscard]] constexpr details::value_or_reference_return_t<
      T>
      get() const noexcept(noexcept(
        details::value_or_reference_return_t<T>{ std::declval<T &>() }))
    {
      return this->ptr_;
    }

    [[deprecated]]
    constexpr decltype(auto) operator->() const noexcept(false)
    {
      if (nullptr == this->ptr_) { throw nullptr_exception(); }
      return get();
    }

    [[deprecated]]
    constexpr decltype(auto) operator*() const
    {
      if (nullptr == this->ptr_) { throw nullptr_exception(); }
      return *get();
    }

    template<Pointer U> friend class maybe_null;
  };


  template<class T> [[nodiscard]] constexpr auto make_maybe_null(T &&t) noexcept
  {
    return maybe_null<std::remove_cv_t<std::remove_reference_t<T>>>{
      std::forward<T>(t)
    };
  }


#if (defined(__cpp_deduction_guides) && (__cpp_deduction_guides >= 201611L))

  // deduction guides to prevent the ctad-maybe-unsupported warning
  template<class T> maybe_null(T) -> maybe_null<T>;

#endif// ( defined(__cpp_deduction_guides) && (__cpp_deduction_guides >=
      // 201611L) )

  //////////////////////////////////////////////
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
  ////////////////////////////////////////////////////////////////////////////

  template<Pointer T> class borrower : public wrapped_pointer<T>
  {
  public:
    explicit borrower()
      requires std::is_default_constructible_v<T>
    {}

    explicit borrower(std::nullptr_t) noexcept
      requires(Nullable<T>)
      : wrapped_pointer<T>(nullptr)
    {}

    explicit borrower(T ptr) : wrapped_pointer<T>(ptr) {}

    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    borrower(borrower<U> const &other) noexcept
      : wrapped_pointer<T>(other.get())
    {}

    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    explicit borrower(borrower<U> &&other) noexcept
      : wrapped_pointer<T>(other.get())
    {
      static_assert(
        std::is_rvalue_reference<decltype(other)>::value, "Must be an rvalue");
      // Owner cannot move pointer to borrower.
      // Borrower cannot move pointer to owner.
    }

    template<Pointer U>
    explicit borrower(owner<U> const &other)
      requires(std::is_convertible<U, T>::value);

    ~borrower() = default;

    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    borrower<T> &operator=(borrower<U> const &other) noexcept
    {
      // TODO borrower accepts owner
      if (this == other) { return *this; }
      this->ptr_ = other.get();
      return *this;
    }

    /** No operator to prevent the pointer escaping, and a constructor
     * called with the pointer.
     */
    //    operator T() const = delete;

    constexpr decltype(auto) operator->() const { return this->get(); }
    constexpr decltype(auto) operator*() const { return *(this->get()); }
#ifdef TODO
    constexpr operator bool() const { return this->get() != nullptr; }
#endif


    template<Pointer U> friend inline borrower<U> make_borrower(U ptr);
  };

  template<Pointer T> inline borrower<T> make_borrower(T ptr)
  {
    return borrower<T>(ptr);
  }


  ////////////////////////////////////////////////////////////////////////////
  //
  // owner
  //
  // Indicates that the pointer resource should be removed.
  // Should be migrated to std::unique_ptr, std::shared_ptr, or
  // std::weak_ptr.  Provided as a step in transforming a legacy
  // program.
  //
  // Has zero size overhead over T.
  //
  // If T is a pointer (i.e. T == U*) then
  // - allow construction from U*
  // - allow implicit conversion to U*
  //
  // based on gsl::not_null
  ////////////////////////////////////////////////////////////////////////////


  template<Pointer T> class owner : public wrapped_pointer<T>
  {
  public:
    explicit owner()
      requires std::is_default_constructible_v<T>
    {
      ;
    }

    explicit owner(std::nullptr_t) noexcept
      requires(Nullable<T>)
      : wrapped_pointer<T>(nullptr)
    {}

    explicit owner(T ptr) : wrapped_pointer<T>(ptr) {}

    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    explicit owner(owner<U> &&other) : wrapped_pointer<T>(std::forward(other))
    {}

    ~owner() = default;

    template<typename U,
      typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    owner<T> &operator=(owner<U> const &other) noexcept
    {
      // TODO borrower accepts owner
      if (this == other) { return *this; }
      this->ptr_ = other.get();
      return *this;
    }

    constexpr decltype(auto) operator->() const { return this->get(); }
    constexpr decltype(auto) operator*() const { return *this->get(); }
#ifdef TODO
    constexpr operator bool() const { return this->get() != nullptr; }
#endif

    borrower<T> as_borrower() const { return borrower<T>(this->get()); }

    template<Pointer U> friend inline owner<U> make_owner(U ptr);
  };


#if !defined(MP_NO_IOSTREAMS)
  template<class T>
  std::ostream &operator<<(std::ostream &os, owner<T> const &val)
  {
    os << val.get();
    return os;
  }
#endif// !defined(MP_NO_IOSTREAMS)

  template<Pointer T> inline owner<T> make_owner(T ptr)
  {
    return owner<T>(ptr);
  }

  template<Pointer T>
  template<Pointer U>
  borrower<T>::borrower(owner<U> const &theOwner)
    requires(std::is_convertible<U, T>::value)
    : wrapped_pointer<T>(theOwner.ptr_)
  {}

  template<class T, std::enable_if_t<std::is_pointer<T>::value, bool> = true>
  using nonowner = T;

}// namespace pointers
}// namespace marcpawl
