#pragma once
#include <array>
#include <string>

template <typename T>
static consteval T template_max(const T& lhs, const T& rhs);

template <size_t size> class BigInt {
private:
  bool negative = false;
  const size_t order = size;
  std::array<short, size> repr{};

public:
  BigInt() {};
  ~BigInt() {};

  BigInt(long);
  BigInt(std::array<short, size>&&);
  BigInt(BigInt<size>&&) noexcept;
  BigInt(const BigInt<size>&) noexcept;

  BigInt& operator=(const BigInt<size>&);
  BigInt& operator=(BigInt<size>&&) noexcept;

  constexpr const size_t num_order() const noexcept { return order; };
  const std::array<short, size>& get_repr() const noexcept { return repr; };
  const bool is_negative() const noexcept { return negative; }
  void neg() noexcept { negative = !negative; }

  template <size_t other_size>
  BigInt<template_max(size, other_size)> operator+=(BigInt<other_size> rhs);

  template <size_t other_size>
  BigInt<template_max(size, other_size)> operator-=(BigInt<other_size> rhs);

  const short operator[](size_t index) const;
  short& operator[](size_t index);
  BigInt<size>& operator<<(size_t);

  operator std::string() const;
  const short last() const;
};
