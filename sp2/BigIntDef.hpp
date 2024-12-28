#pragma once
#include <array>
#include <string>

template <typename T>
static consteval T template_max(const T& lhs, const T& rhs);

template <std::size_t size> class BigInt {
private:
  bool negative = false;
  const std::size_t order = size;
  std::array<short, size> repr{};

public:
  BigInt();
  ~BigInt();

  BigInt(long);
  BigInt(std::array<short, size>&&);
  BigInt(BigInt<size>&&) noexcept;
  BigInt(const BigInt<size>&) noexcept;

  BigInt& operator=(const BigInt<size>&);
  BigInt& operator=(BigInt<size>&&) noexcept;

  constexpr const std::size_t num_order() const noexcept;
  const std::array<short, size>& get_repr() const noexcept;
  const bool is_negative() const;

  template <std::size_t other_size>
  BigInt<template_max(size, other_size)> operator+=(BigInt<other_size> rhs);

  template <std::size_t other_size>
  BigInt<template_max(size, other_size)> operator-=(BigInt<other_size> rhs);

  BigInt<size>& operator-();
  const short operator[](std::size_t index) const;
  short& operator[](std::size_t index);
  BigInt<size>& operator<<(std::size_t);

  operator std::string() const;
  const short last() const;
};
