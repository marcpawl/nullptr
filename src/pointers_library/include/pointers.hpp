
#pragma once

#include <type_traits>
#include <gsl/gsl>

namespace marcpawl {
namespace pointers {
template <class T, std::enable_if_t<std::is_pointer<T>::value, bool> = true>
using nonowner = T;

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
template <class T>
class borrower
{
public:
    static_assert(std::is_pointer<T>::value, "T Must be pointer.");

    explicit borrower() = default;

    explicit borrower(T ptr) noexcept : ptr_(ptr) {}
    
    template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>  
    explicit borrower(borrower<U> const& ptr) noexcept 
        : ptr_(ptr.get())
        {}

     template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>  
        explicit  borrower(borrower<U>&& other) noexcept
          : ptr_(other.get())
          {}

~borrower() = default;

template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>  
borrower<T>& operator=(borrower<U> const& other) noexcept
  {
    if (this == other) { return *this; }
    ptr_ = other.get();
    return *this;
  }

    constexpr T get() const noexcept {
        return ptr_;
    }

//    constexpr operator T() const = delete;
    constexpr decltype(auto) operator->() const { return get(); }
    constexpr decltype(auto) operator*() const { return *get(); }

    // unwanted operators...pointers only point to single objects!
    borrower& operator++() = delete;
    borrower& operator--() = delete;
    borrower operator++(int) = delete;
    borrower operator--(int) = delete;
    borrower& operator+=(std::ptrdiff_t) = delete;
    borrower& operator-=(std::ptrdiff_t) = delete;
    void operator[](std::ptrdiff_t) const = delete;

    void operator delete(void*, size_t) = delete;

private:
    T ptr_ = nullptr;
};

template <class T>
auto make_borrower(T&& t) noexcept
{
    return borrower<std::remove_cv_t<std::remove_reference_t<T>>>{std::forward<T>(t)};
}

#if !defined(GSL_NO_IOSTREAMS)
template <class T>
std::ostream& operator<<(std::ostream& os, const borrower<T>& val)
{
    os << val.get();
    return os;
}
#endif // !defined(GSL_NO_IOSTREAMS)

template <class T, class U>
auto operator==(const borrower<T>& lhs,
                const borrower<U>& rhs) noexcept(noexcept(lhs.get() == rhs.get()))
    -> decltype(lhs.get() == rhs.get())
{
    return lhs.get() == rhs.get();
}

template <class T, class U>
auto operator!=(const borrower<T>& lhs,
                const borrower<U>& rhs) noexcept(noexcept(lhs.get() != rhs.get()))
    -> decltype(lhs.get() != rhs.get())
{
    return lhs.get() != rhs.get();
}

template <class T, class U>
auto operator<(const borrower<T>& lhs,
               const borrower<U>& rhs) noexcept(noexcept(std::less<>{}(lhs.get(), rhs.get())))
    -> decltype(std::less<>{}(lhs.get(), rhs.get()))
{
    return std::less<>{}(lhs.get(), rhs.get());
}

template <class T, class U>
auto operator<=(const borrower<T>& lhs,
                const borrower<U>& rhs) noexcept(noexcept(std::less_equal<>{}(lhs.get(), rhs.get())))
    -> decltype(std::less_equal<>{}(lhs.get(), rhs.get()))
{
    return std::less_equal<>{}(lhs.get(), rhs.get());
}

template <class T, class U>
auto operator>(const borrower<T>& lhs,
               const borrower<U>& rhs) noexcept(noexcept(std::greater<>{}(lhs.get(), rhs.get())))
    -> decltype(std::greater<>{}(lhs.get(), rhs.get()))
{
    return std::greater<>{}(lhs.get(), rhs.get());
}

template <class T, class U>
auto operator>=(const borrower<T>& lhs,
                const borrower<U>& rhs) noexcept(noexcept(std::greater_equal<>{}(lhs.get(), rhs.get())))
    -> decltype(std::greater_equal<>{}(lhs.get(), rhs.get()))
{
    return std::greater_equal<>{}(lhs.get(), rhs.get());
}

// more unwanted operators
template <class T, class U>
std::ptrdiff_t operator-(const borrower<T>&, const borrower<U>&) = delete;
template <class T>
borrower<T> operator-(const borrower<T>&, std::ptrdiff_t) = delete;
template <class T>
borrower<T> operator+(const borrower<T>&, std::ptrdiff_t) = delete;
template <class T>
borrower<T> operator+(std::ptrdiff_t, const borrower<T>&) = delete;


template <class T, class U = decltype(std::declval<const T&>().get()), bool = std::is_default_constructible<std::hash<U>>::value>
struct borrower_hash
{
    std::size_t operator()(const T& value) const { return std::hash<U>{}(value.get()); }
};

template <class T, class U>
struct borrower_hash<T, U, false>
{
    borrower_hash() = delete;
    borrower_hash(const borrower_hash&) = delete;
    borrower_hash& operator=(const borrower_hash&) = delete;
};

}
}

