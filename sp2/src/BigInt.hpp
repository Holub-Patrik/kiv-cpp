#include <algorithm>
#include <array>
#include <cmath>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <utility>

#include "BigIntDef.hpp"
#include "BigNumberException.hpp"

template <typename T> consteval T template_max(const T& lhs, const T& rhs) {
  if (lhs < rhs) {
    return lhs;
  } else {
    return rhs;
  }
}

template <typename T>
concept SizeParam = std::convertible_to<T, uint64_t> && requires(T t) {
  { t + t } -> std::convertible_to<T>;
  { t - t } -> std::convertible_to<T>;
  { t % t } -> std::convertible_to<T>;
  { t / t } -> std::convertible_to<T>;
};

template <typename T>
concept TrivialIntegral =
    std::integral<T> && requires(T t) { t >= 0 && t < 10; };

template <size_t size>
BigInt<size>::BigInt(long num) : repr(std::array<short, size>{}) {
  negative = num < 0;
  if (negative) {
    num *= -1;
  }

  for (int i = 0; num > 0; ++i) {
    if (i >= size) {
      throw BigNumberException<size>(
          "Number too large for specified template size");
    }
    repr[i] = num % 10;
    num /= 10;
  }
}

template <size_t size>
BigInt<size>::BigInt(std::array<short, size>&& repr) : repr(repr) {}

// move constructor
template <size_t size>
BigInt<size>::BigInt(BigInt<size>&& other) noexcept
    : repr(std::move(other.repr)), negative(other.negative) {}

// copy constructor
template <size_t size>
BigInt<size>::BigInt(const BigInt<size>& other) noexcept {
  negative = other.negative;
  repr = other.repr;
}

// copy assignment
template <size_t size>
BigInt<size>& BigInt<size>::operator=(const BigInt<size>& other) {
  if (this == &other)
    return *this;

  BigInt<size> temp(other);
  std::swap(repr, temp.repr);
  negative = temp.negative;

  return *this;
}

// move assignment
template <size_t size>
BigInt<size>& BigInt<size>::operator=(BigInt<size>&& other) noexcept {
  BigInt<size> temp(std::move(other));
  std::swap(repr, temp.repr);
  negative = temp.negative;
  return *this;
}

template <size_t size> BigInt<size>::operator std::string() const {
  std::ostringstream s;
  if (negative) {
    s << "-";
  }
  bool filler = true;

  std::for_each(repr.rbegin(), repr.rend(), [&s, &filler](auto elem) {
    if (elem != 0 || !filler) {
      s << elem;
      // s << " ";
      filler = false;
    }
  });

  if (filler) {
    s << "0";
  }

  return std::string{s.str()};
}

template <size_t size>
std::ostream& operator<<(std::ostream& os, const BigInt<size>& num) {
  os << static_cast<std::string>(num);
  return os;
}

template <size_t size>
const short BigInt<size>::operator[](size_t index) const {
  return repr[index];
}

template <size_t size> short& BigInt<size>::operator[](size_t index) {
  return repr[index];
}

template <size_t size> const short BigInt<size>::last() const {
  return *repr.rbegin();
}

template <size_t size>
BigInt<size>& BigInt<size>::operator<<(size_t shift_amount) {
  for (int i = size - 1; i >= static_cast<int>(shift_amount); --i) {
    repr[i] = repr[i - shift_amount];
  }

  for (size_t i = 0; i < shift_amount; ++i) {
    repr[i] = 0;
  }
  return *this;
}

template <size_t size>
template <size_t other_size>
BigInt<size> BigInt<size>::operator+=(const BigInt<other_size>& rhs) {
  if (other_size > size) {
    for (size_t i = size; i < other_size; i++) {
      if (rhs[i]) {
        throw BigNumberException<size>("Overflow [+=] before");
      }
    }
  }

  const bool lhs_neg = negative;
  const bool rhs_neg = rhs.is_negative();
  const auto lr_cmp = *this <=> rhs;

  if (!lhs_neg && rhs_neg) {
    return *this -= -rhs;
  }

  if (lhs_neg && !rhs_neg) {
    return (this->neg() -= rhs).neg();
  }

  if (lhs_neg && rhs_neg) {
    return (this->neg() += -rhs).neg();
  }

  short carry = 0;
  auto index_gen = std::views::iota(0);
  auto index = std::views::counted(index_gen.begin(), size);

  std::for_each(index.begin(), index.end(), [this, &rhs, &carry](auto i) {
    const short lhs_num = i < size ? repr[i] : 0;
    const short rhs_num = i < other_size ? rhs[i] : 0;
    const short temp = lhs_num + rhs_num + carry;
    repr[i] = temp % 10;
    carry = temp / 10;
  });

  if (carry) {
    throw BigNumberException<size>("Overflow [+=] carry");
  }

  return *this;
}

template <size_t lhs_size, size_t rhs_size>
BigInt<template_max(lhs_size, rhs_size)>
operator+(const BigInt<lhs_size>& lhs, const BigInt<rhs_size>& rhs) {
  constexpr const size_t resulting_size = template_max(lhs_size, rhs_size);

  // addition implementation works only for positive numbers
  // so I just transform everything into addition/subtraction of positive
  // numbers
  const bool lhs_neg = lhs.is_negative();
  const bool rhs_neg = rhs.is_negative();
  const auto lr_cmp = lhs <=> rhs;

  if (!lhs_neg && rhs_neg) {
    return lhs - -rhs;
  }

  if (lhs_neg && !rhs_neg) {
    return -(-lhs - rhs);
  }

  if (lhs_neg && rhs_neg) {
    return -(-lhs + -rhs);
  }

  std::array<short, resulting_size> res_arr;
  short carry = 0;
  auto index_gen = std::views::iota(0);
  auto index = std::views::counted(index_gen.begin(), resulting_size);

  std::for_each(index.begin(), index.end(),
                [&carry, &res_arr, &rhs, &lhs](auto i) {
                  const short lhs_num = i < lhs_size ? lhs[i] : 0;
                  const short rhs_num = i < rhs_size ? rhs[i] : 0;
                  const short temp = lhs_num + rhs_num + carry;
                  res_arr[i] = temp % 10;
                  carry = temp / 10;
                });

  if (carry) {
    throw BigNumberException<resulting_size>("Overflow [+] carry");
  }

  return BigInt<resulting_size>{std::move(res_arr)};
}

template <size_t size>
template <size_t other_size>
BigInt<size> BigInt<size>::operator-=(const BigInt<other_size>& rhs) {
  const bool lhs_neg = negative;
  const bool rhs_neg = rhs.is_negative();
  const auto lr_cmp = *this <=> rhs;

  if (lr_cmp == 0) {
    return BigInt<size>{0};
  }

  if (!lhs_neg && rhs_neg) {
    return *this += -rhs;
  }

  if (lhs_neg && !rhs_neg) {
    return ((*this).neg() += rhs).neg();
  }

  if (lhs_neg && rhs_neg) {
    return ((*this).neg() -= -rhs).neg();
  }

  short carry = 0;
  for (size_t i = 0; i < size; ++i) {
    const short lhs_num = i < size ? repr[i] : 0;
    const short rhs_num = i < other_size ? rhs[i] + carry : 0;
    short temp;

    if (lr_cmp < 0) {
      temp = rhs_num - lhs_num;
    } else {
      temp = lhs_num - rhs_num;
    }

    if (temp >= 0) {
      repr[i] = temp;
      carry = 0;
    } else {
      repr[i] = temp + 10;
      carry = 1;
    }
  }

  if (lr_cmp < 0) {
    negative = !negative;
  }

  return *this;
}

template <size_t size> BigInt<size> operator-(const BigInt<size>& num) {
  auto new_num = BigInt<size>(num);
  new_num.neg();
  return new_num;
}

template <size_t lhs_size, size_t rhs_size>
BigInt<template_max(lhs_size, rhs_size)>
operator-(const BigInt<lhs_size>& lhs, const BigInt<rhs_size>& rhs) {
  constexpr const size_t resulting_size = template_max(lhs_size, rhs_size);

  // subtraction implementation works only for positive numbers
  // so I created these rules to convert everything into addition/subtraction of
  // positive numbers
  const bool lhs_neg = lhs.is_negative();
  const bool rhs_neg = rhs.is_negative();
  const auto lr_cmp = lhs <=> rhs;

  if ((lhs <=> rhs) == 0) {
    return BigInt<resulting_size>{0};
  }

  if (!lhs_neg && !rhs_neg) {
    if ((lr_cmp) < 0) {
      return -(rhs - lhs);
    }
  }

  if (!lhs_neg && rhs_neg) {
    return lhs + -rhs;
  }

  if (lhs_neg && !rhs_neg) {
    return -(-lhs + rhs);
  }

  if (lhs_neg && rhs_neg) {
    return -(-lhs - -rhs);
  }

  std::array<short, resulting_size> res_arr;
  short carry = 0;
  for (size_t i = 0; i < resulting_size; ++i) {
    short lhs_num = i < lhs_size ? lhs[i] : 0;
    short rhs_num = i < rhs_size ? rhs[i] + carry : 0;
    if (lhs_num - rhs_num >= 0) {
      res_arr[i] = lhs_num - rhs_num;
      carry = 0;
    } else {
      res_arr[i] = (lhs_num + 10) - rhs_num;
      carry = 1;
    }
  }

  return BigInt<resulting_size>{std::move(res_arr)};
}

template <size_t size>
BigInt<size> operator*(const BigInt<size>& num,
                       const TrivialIntegral auto mul) {
  std::array<short, size> res_arr;
  short carry = 0;
  auto index_gen = std::views::iota(0);
  auto index = std::views::counted(index_gen.begin(), size);

  std::for_each(index.begin(), index.end(),
                [&carry, &res_arr, &num, &mul](auto i) {
                  const short temp = num[i] * mul + carry;
                  res_arr[i] = temp % 10;
                  carry = temp / 10;
                });
  if (carry) {
    throw BigNumberException<size>("Overflow [*] carry");
  }

  return BigInt<size>{std::move(res_arr)};
}

template <size_t lhs_size, size_t rhs_size>
BigInt<template_max(lhs_size, rhs_size)>
operator*(const BigInt<lhs_size>& lhs, const BigInt<rhs_size>& rhs) {
  constexpr const size_t resulting_size = template_max(lhs_size, rhs_size);

  BigInt<resulting_size> res_num{};
  for (size_t i = 0; i < resulting_size; ++i) {
    auto temp_num = lhs * rhs[i];
    if ((temp_num <=> BigInt<1>{0}) != 0) {
      res_num += temp_num << i;
    }
  }

  if (lhs.is_negative() && !rhs.is_negative() ||
      !lhs.is_negative() && rhs.is_negative()) {
    res_num.neg();
  }

  return res_num;
}

template <size_t size>
template <size_t other_size>
BigInt<size> BigInt<size>::operator*=(const BigInt<other_size>& rhs) {
  BigInt<size> res_num{};
  for (size_t i = 0; i < size; ++i) {
    auto temp_num = *this * rhs[i];
    if ((temp_num <=> BigInt<1>{0}) != 0) {
      res_num += temp_num << i;
    }
  }

  if (negative && !rhs.is_negative() || !negative && rhs.is_negative()) {
    res_num.neg();
  }

  repr = std::move(res_num.repr);
  negative = res_num.negative;

  return *this;
}

template <size_t lhs_size, size_t rhs_size>
BigInt<template_max(lhs_size, rhs_size)>
operator/(const BigInt<lhs_size>& lhs, const BigInt<rhs_size>& rhs) {
  constexpr const size_t resulting_size = template_max(lhs_size, rhs_size);

  BigInt<resulting_size> res{0};
  auto temp = lhs.abs();
  auto sub_abs = rhs.abs();
  while ((temp <=> rhs) > 0) {
    res += BigInt<resulting_size>{1};
    temp -= sub_abs;
  }

  if (lhs.is_negative() && !rhs.is_negative() ||
      !lhs.is_negative() && rhs.is_negative()) {
    res.neg();
  }

  return res;
}

template <size_t size>
template <size_t other_size>
BigInt<size> BigInt<size>::operator/=(const BigInt<other_size>& rhs) {
  BigInt<size> res{0};
  auto temp = BigInt<size>{*this}.abs();
  auto sub_abs = rhs.abs();

  while ((temp <=> rhs) > 0) {
    res += BigInt<size>{1};
    temp -= sub_abs;
  }

  if (negative && !rhs.is_negative() || !negative && rhs.is_negative()) {
    res.neg();
  }

  repr = std::move(res.repr);
  negative = res.negative;

  return *this;
}

template <size_t lhs_size, size_t rhs_size>
auto operator<=>(const BigInt<lhs_size>& lhs, const BigInt<rhs_size>& rhs) {
  // check if one or the other is negative while the other isn't
  // this means later that checking just one is fine
  if (lhs.is_negative() && !rhs.is_negative()) {
    return std::strong_ordering::less;
  } else if (!lhs.is_negative() && rhs.is_negative()) {
    return std::strong_ordering::greater;
  }

  // find index od the largest number in the array
  size_t lhs_index = 0;
  size_t rhs_index = 0;

  for (size_t i = lhs_size; i--;) {
    if (lhs[i]) {
      lhs_index = i;
      break;
    }
  }

  for (size_t i = rhs_size; i--;) {
    if (rhs[i]) {
      rhs_index = i;
      break;
    }
  }

  // if indexes aren't equal, compare the indexes which is the same as
  // comparing which number has the greater order of magnitude
  //
  // otherwise go down the numbers to find the first non-matching
  // if no non-matching is found, return equal
  std::strong_ordering order = std::strong_ordering::less;
  if (lhs_index != rhs_index) {
    order = lhs_index <=> rhs_index;
  } else {
    bool order_set = false;
    int i; // size_t here breaks the loop
    for (i = lhs_index; i >= 0; --i) {
      if (lhs[i] != rhs[i]) {
        order = lhs[i] <=> rhs[i];
        order_set = true;
        break;
      }
    }

    if (!order_set) {
      return std::strong_ordering::equal;
    }
  }

  // if the numbers were negative, the results needs to be flipped
  if (lhs.is_negative()) {
    order = 0 <=> order;
  }

  return order;
}

template <size_t size> BigInt<size> BigInt<size>::factorial() const {
  BigInt<size> res{1};

  for (BigInt<size> i{1}; (i <=> *this) <= 0; i += BigInt<size>{1}) {
    res *= i;
  }

  return res;
}
