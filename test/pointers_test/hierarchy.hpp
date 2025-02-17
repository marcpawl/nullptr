#pragma once


struct Parent
{
  int *value;
  Parent()
  {
    // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
    value = new int(0);
  }
  Parent(Parent &other) = delete;
  Parent(Parent &&other) noexcept
  {
    // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
    delete value;
    // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
    value = new int(other.get_value());
    // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
    delete other.value;
    other.value = nullptr;
  }
  virtual ~Parent() { delete value; }

  Parent &operator=(Parent &other) = delete;
  Parent &operator=(Parent &&other) noexcept
  {
    if (this == &other) { return *this; }
    // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
    delete value;
    // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
    value = new int(other.get_value());
    // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
    delete other.value;
    other.value = nullptr;
    return *this;
  }

  [[nodiscard]]
  int get_value() const
  {
    if (nullptr == value) { return 0; }
    return *value;
  }
};

struct Child : Parent
{
  int another_value = 0;

  Child() = default;
  Child(Child &other) = delete;
  Child(Child &&other) = default;
  ~Child() override = default;
  Child &operator=(Child &other) = delete;
  Child &operator=(Child &&other) = default;
};

constexpr int legacy(int const*const ptr)
{
  if (nullptr == ptr) { return -1; }
  return *ptr;
}

